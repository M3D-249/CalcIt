#!/usr/bin/bash

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

install_dependencies () {
    print_status "Installing system dependencies..."

    sudo pacman -S --needed --noconfirm base-devel git meson ninja

    sudo pacman -S --needed --noconfirm clang gcc gdb

    print_status "System dependencies installed."
}


setup_build () {
    print_status "Setting up build directory..."

    mkdir -p "${BUILD_DIR}"
    cd "${BUILD_DIR}" || print_error "Couldnt create build directory"

    meson setup .. build \
    --prefix="${INSTALL_PREFIX}" \
    --buildtype="${BUILD_TYPE}" \
    -Dcpp_std=c++23 \
    -Dwarning_level=3 \
    -Db_lto=true \

    cd ..

    print_status "Build system configured"
}


build_project() {
    print_status "Installing project to ${INSTALL_PREFIX}..."

    cd "${BUILD_DIR}" || print_error "Build directory not found"
    ninja
    cd ..

    print_status "Project installed"
}


create_env_script() {
    print_status "Creating environment script..."

    cat > env.sh << EOF    
[settings]
name=value
EOF

#!/usr/bin/env

export PATH="${INSTALL_PREFIX}/bin:$PATH"

    chmod +x env.sh

    print_status "Environment script created: source ./env.sh to activate"
}

add_to_profile() {
    local profile_file="${HOME}/.bashrc"

    if ! grep -q "${PROJECT_NAME} environment" "${profile_file}"; then
        print_status "Adding environemnt setup to ${profile_file}"
        echo "" >> "${profile_file}"
        echo "# ${PROJECT_NAME} environment" >> "${profile_file}"
        echo "source ${PWD}/env.sh 2>/dev/null || true" >> "${profile_file}"
    fi
}


main() {
    print_status "Starting setup for ${PROJECT_NAME}"

    echo "select build type : " "${BUILD_TYPES[@]}"
    read -r -p "type : " type
    BUILD_TYPE=$type

    install_dependencies

    setup_build
    build_project

    create_env_script

    read -r -n 1 -p "Add environment to your shell profile? (y/n): " REPLY
    echo
    if [ "$REPLY" = "y" ]; then
        add_to_profile
    fi

    print_status "Setup Complete!"
    print_warning "Run: source ./env.sh to activate the environment"
    print_warning "Or logout and back for permenant changes."
}

main "$@"