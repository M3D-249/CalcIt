#!/usr/bin/env bash

set -euo pipefail

PROJECT_NAME="CalcIt"
BUILD_DIR="build"
INSTALL_PREFIX="${HOME}/.local/${PROJECT_NAME}"

BUILD_TYPES=("debug" "debugoptimized" "release")
BUILD_TYPE="debugoptimized"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m'

declare -A pkg_names=(
    ["git"]="git"
    ["gcc"]="gcc"
    ["gdb"]="gdb"
    ["clang"]="clang"
    ["ninja"]="ninja-build"
    ["meson"]="meson"
)

print_status() {
    echo -e "${GREEN}==>${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[!]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
    exit 1
}

detect_pkg_manager () {
    if command -v apt-get &> /dev/null; then
        echo "apt"
    elif command -v dnf &> /dev/null; then
        echo "dnf"
    elif command -v yum &> /dev/null; then
        echo "yum"
    elif command -v pacman &> /dev/null; then
        echo "pacman"
    elif command -v zypper &> /dev/null; then
        echo "zypper"
    elif command -v apk &> /dev/null; then
        echo "apk"
    else
        print_error "No supported package manager found."
    fi
}

is_installed () {
    command -v "$1" &>/dev/null
}

install_dependencies () {
    print_status "Installing system dependencies..."

    local pkg_manager
    pkg_manager=$(detect_pkg_manager)
    print_status "Package manager detected: ${pkg_manager}"

    # Adjust for package manager naming
    case $pkg_manager in
        "pacman")
            pkg_names["ninja"]="ninja"
            ;;
        "apk")
            pkg_names["ninja"]="ninja"
            pkg_names["meson"]="meson py3-setuptools"
            ;;
    esac

    # Only add missing tools
    local pkgs=()
    for tool in git clang gcc gdb ninja meson; do
        if ! is_installed "$tool"; then
            pkgs+=("${pkg_names[$tool]}")
        fi
    done

    if [ ${#pkgs[@]} -eq 0 ]; then
        print_status "All required packages are already installed."
        return
    fi

    case $OSTYPE in
        linux*)
            case $pkg_manager in
                "apt")
                    sudo apt update
                    sudo apt install -y "${pkgs[@]}"
                    ;;
                "dnf" | "yum")
                    sudo "$pkg_manager" check-update || true
                    sudo "$pkg_manager" install -y "${pkgs[@]}"
                    ;;
                "pacman")
                    sudo pacman -Sy --noconfirm "${pkgs[@]}"
                    ;;
                "zypper")
                    sudo zypper refresh
                    sudo zypper install -y "${pkgs[@]}"
                    ;;
                "apk")
                    sudo apk update
                    sudo apk add --no-cache "${pkgs[@]}"
                    ;;
            esac
            ;;
        msys*)
            if command -v choco &> /dev/null; then
                choco install "${pkgs[@]}"
            else
                powershell.exe -NoProfile -InputFormat None -ExecutionPolicy Bypass \
                    -Command "[System.Net.ServicePointManager]::SecurityProtocol = 3072; \
                    iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
                export PATH="$PATH:/c/ProgramData/chocolatey/bin"
            fi
            ;;
        *)
            print_error "Unsupported OS: $OSTYPE"
            ;;
    esac

    print_status "System dependencies installed."
}

setup_build () {
    print_status "Setting up build directory..."

    mkdir -p "${BUILD_DIR}"

    meson setup "${BUILD_DIR}" \
        --prefix="${INSTALL_PREFIX}" \
        --buildtype="${BUILD_TYPE}" \
        -Dcpp_std=c++23 \
        -Dwarning_level=3 \
        -Db_lto=true

    print_status "Build system configured"
}

build_project() {
    print_status "Building project..."

    cd "${BUILD_DIR}" || print_error "Build directory not found"
    ninja
    ninja install
    cd ..

    print_status "Project built and installed"
}

create_env_script() {
    print_status "Creating environment script..."

    cat > env.sh << EOF
#!/usr/bin/env bash

export PATH="${INSTALL_PREFIX}/bin:\$PATH"
EOF

    chmod +x env.sh
    print_status "Environment script created: source ./env.sh to activate"
}

add_to_profile() {
    local profile_file="${HOME}/.bashrc"

    if ! grep -q "${PROJECT_NAME} environment" "${profile_file}"; then
        print_status "Adding environment setup to ${profile_file}"
        {
            echo ""
            echo "# ${PROJECT_NAME} environment"
            echo "source ${PWD}/env.sh 2>/dev/null || true"
        } >> "${profile_file}"
    fi
}

main() {
    print_status "Starting setup for ${PROJECT_NAME}"

    echo "Select build type:"
    for ((i=0; i<${#BUILD_TYPES[@]}; i++)); do
        echo "$((i+1)). ${BUILD_TYPES[$i]}"
    done

    read -r -p "Type (number): " type_index

    if [[ "$type_index" =~ ^[0-9]+$ ]] && (( type_index >= 1 && type_index <= ${#BUILD_TYPES[@]} )); then
        BUILD_TYPE=${BUILD_TYPES[$((type_index - 1))]}
    else
        print_error "Invalid build type selection."
    fi

    install_dependencies
    setup_build
    build_project
    create_env_script

    read -r -n 1 -p "Add environment to your shell profile? (y/n): " REPLY
    echo
    if [[ "$REPLY" =~ ^[Yy]$ ]]; then
        add_to_profile
    fi

    print_status "Setup Complete!"
    print_warning "Run: source ./env.sh to activate the environment"
    print_warning "Or log out and back in for permanent changes."
}

main "$@"
