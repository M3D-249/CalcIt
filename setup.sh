#!/usr/bin/env bash

set -euo pipefail

PROJECT_NAME="CalcIt"
BUILD_DIR="build"
INSTALL_PREFIX="${HOME}/.local/${PROJECT_NAME}"


if [ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin"]; then
    if [[ "${INSTALL_PREFIX}" == /* ]]; then
        INSTALL_PREFIX="$(cygpath -w "${INSTALL_PREFIX}" 2>/dev/null || echo "${INSTALL_PREFIX}")"
    fi
fi

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
    ["ninja"]="ninja-build"
    ["meson"]="meson"
)

case "$OSTYPE" in 
    msys*)
        pkg_names["ninja"] = "ninja"
        pkg_names["meson"] = "meson"
        ;;
esac

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
    case "$OSTYPE" in
        linux*)
            if is_installed apt-get; then
                echo "apt"
            elif is_installed dnf; then
                echo "dnf"
            elif is_installed yum; then
                echo "yum"
            elif is_installed pacman; then
                echo "pacman"
            elif is_installed zypper; then
                echo "zypper"
            elif is_installed apk; then
                echo "apk"
            else
                print_error "No supported package manager found."
            fi
            ;;
        msys*)
            echo "choco"
            ;;
        cygwin*)
            echo "choco"
            ;;
        *)
            print_error "Unsupported OS: $OSTYPE"
            ;;
    esac
}

is_installed () {
    command -v "$1" &>/dev/null
}

install_chocolatey() {
    powershell.exe -NoProfile -ExecutionPolicy Bypass \
        -Command "[System.Net.ServicePointManager]::SecurityProtocol = 3072; Set-ExecutionPolicy Bypass -Scope Process -Force; \
        iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
    export PATH="$PATH:/c/ProgramData/chocolatey/bin"
}

install_dependencies () {
    print_status "Installing system dependencies..."

    local pkg_manager
    pkg_manager=$(detect_pkg_manager)
    print_status "Package manager detected: ${pkg_manager}"

    case $pkg_manager in
        "apt" | "dnf" | "yum" | "pacman" | "zypper" | "apk")
            case $pkg_manager in
                "pacman")
                    pkg_names["ninja"]="ninja"
                    ;;
                "apk")
                    pkg_names["ninja"]="ninja"
                    pkg_names["meson"]="meson py3-setuptools"
                    ;;
            esac

            local pkgs=()
            for tool in git gcc gdb ninja meson; do
                if ! is_installed "$tool"; then
                    pkgs+=("${pkg_names[$tool]}")
                fi
            done

            if [ ${#pkgs[@]} -eq 0 ]; then
                print_status "All required pkgs are installed!"
                return
            fi

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
        "choco")
            if ! is_installed choco; then
                check_admin() {
                    powershell.exe -Command "(New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)" | grep -q "True"
                }
                
                install_chocolatey() {
                    powershell.exe -NoProfile -ExecutionPolicy Bypass \
                        -Command "[System.Net.ServicePointManager]::SecurityProtocol = 3072; Set-ExecutionPolicy Bypass -Scope Process -Force; \
                        iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))"
                    export PATH="$PATH:/c/ProgramData/chocolatey/bin"
                }
                
                if ! check_admin; then
                    echo "Chocolatey installation requires administrator privileges."
                    echo "Requesting elevation..."
                    
                    powershell.exe -Command "Start-Process -Verb RunAs -FilePath 'bash' -ArgumentList '-c', \"$(printf '%q' "$BASH_SOURCE")\""
                    exit 0
                else
                    echo "Running with administrator privileges..."
                    install_chocolatey
                fi
            fi

            if is_installed choco; then
                echo "Chocolatey installed successfully."
            fi

            declare -A choco_pkgs
            choco_pkgs=(
                ["git"]="git"
                ["gcc"]="mingw"
                ["gdb"]="gdb"
                ["ninja"]="ninja"
                ["meson"]="meson"
            )

            local pkgs=()
            for tool in git gcc gdb ninja meson; do
                if ! is_installed "$tool"; then
                    pkgs+=("${choco_pkgs[$tool]}")
                fi
            done

            if [ ${#pkgs[@]} -gt 0 ]; then
                choco install -y "${pkgs[@]}"
            fi
            ;;
    esac

    print_status "System dependencies installed."
}

setup_build () {
    print_status "Setting up build directory..."

    mkdir -p "${BUILD_DIR}"

    read -r -n 1 -p "Setup For Development ? (y/n): " DEV
    echo
    read -r -n 1 -p "Build Examples ? (y/n): " EXAMPLES
    echo
    read -r -n 1 -p "Build Tests ? (y/n): " TESTS
    echo

    [[ "$DEV" =~ ^[Yy]$ ]] && DEV=true || DEV=false
    [[ "$EXAMPLES" =~ ^[Yy]$ ]] && EXAMPLES=true || EXAMPLES=false
    [[ "$TESTS" =~ ^[Yy]$ ]] && TESTS=true || TESTS=false

    meson setup "${BUILD_DIR}" -Ddev=$DEV -Dexamples=$EXAMPLES -Dtests=$TESTS \
        --prefix="${INSTALL_PREFIX}" \
        --buildtype="${BUILD_TYPE}" \
        --backend=ninja \
        -Dcpp_std=c++17 \
        -Dwarning_level=3 \
        #-Db_lto=true

    print_status "Build system configured"
}

build_project() {
    print_status "Building project..."

    chmod +x "build.sh"
    source build.sh

    # print_status "Project built and installed"
}

create_env_script() {
    print_status "Creating environment script..."

    local install_path="$INSTALL_PREFIX"
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
        install_path=$(cygpath -u "$INSTALL_PREFIX")
    fi

    cat > env.sh << EOF
#!/usr/bin/env bash

export PATH="${INSTALL_PREFIX}/bin:\$PATH"
EOF

    chmod +x env.sh
    print_status "Environment script created: source ./env.sh to activate"
}

add_to_profile() {
    local profile_file

    case "$OSTYPE" in
        msys*)
            profile_file=="${HOME}/.bash_profile"
            ;;
        cygwin*)
            profile_file=="${HOME}/.bash_profile"
            ;;
        *)
            profile_file=="${HOME}/.bashrc"
            ;;
    esac

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
    [[ "$REPLY" =~ ^[Yy]$ ]] && add_to_profile

    print_status "Setup Complete!"
    print_warning "Run: source ./env.sh to activate the environment"
    print_warning "Or log out and back in for permanent changes."
}

main "$@"
