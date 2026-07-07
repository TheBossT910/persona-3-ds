# Persona 3 Dual - Developer Environment
#
# Based on the official devkitPro image so devkitARM is already set up.
# Adds nds-dev (libnds + NDS toolchain), ffmpeg, mtools, and Python deps.
#
# Build:  docker build -t p3d-dev-dev .
# Use:    docker run --rm -it -v "$(pwd)":/project p3d-dev-dev
# Make:   docker run --rm -v "$(pwd)":/project p3d-dev-dev make

FROM devkitpro/devkitarm:latest

LABEL maintainer="P3D Team"
LABEL description="Full build environment for Persona 3 Dual (NDS homebrew)"

# Suppress interactive apt prompts
ENV DEBIAN_FRONTEND=noninteractive

# System packages
# ffmpeg        – video/audio asset conversion (used by the asset pipeline)
# mtools        – FAT image creation (sdcard.img)
# libblas3      – required by ffmpeg at runtime (update-alternatives symlink)
# liblapack3    – same as above
# python3 / pip – asset pipeline scripts
# zip / gzip    – packaging release artifacts
# git-lfs       – large file storage (LFS pointers resolved during CI checkout)
# ccache        – compiler cache for faster rebuilds (CI manages cache via actions/cache)
# gdb-multiarch - debugger
RUN apt-get update && apt-get install -y --no-install-recommends \
    ffmpeg \
    mtools \
    libblas3 \
    liblapack3 \
    python3 \
    python3-pip \
    python3-venv \
    zip \
    gzip \
    git-lfs \
    ccache \
    gdb-multiarch \
    && git lfs install --system \
    && rm -rf /var/lib/apt/lists/*

# NDS toolchain
# devkitARM is already in the base image; nds-dev adds the NDS-specific libs.
RUN dkp-pacman -Sy --noconfirm nds-dev

# Python virtual environment
# The Makefile calls /root/.venv/bin/python3 directly (matching the GitHub
# Actions workflow which creates ~/.venv on the runner, resolving to /root/.venv).
# We create the venv at that exact path so both environments resolve the same way.
COPY tools/requirements.txt /tmp/requirements.txt
RUN python3 -m venv /root/.venv \
    && /root/.venv/bin/pip install --upgrade pip \
    && /root/.venv/bin/pip install -r /tmp/requirements.txt

# Put the venv on PATH so bare `python3` and `pip` also work interactively.
ENV PATH="/root/.venv/bin:$PATH"

# Working directory
# Mount your repo here:  -v "$(pwd)":/project
WORKDIR /project

# Add aigis user so we don't run as root on dev container
RUN useradd -m aigis
RUN chown -R aigis:aigis /opt/devkitpro

# Give sudo access to aigis
RUN echo "aigis ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/aigis \
    && chmod 0440 /etc/sudoers.d/aigis \
    && visudo -c -f /etc/sudoers.d/aigis \
    && rm -rf /var/lib/apt/lists/*

USER aigis

# venv for aigis
RUN python3 -m venv $HOME/.venv \
    && $HOME/.venv/bin/pip install --upgrade pip \
    && $HOME/.venv/bin/pip install -r /tmp/requirements.txt
ENV PATH="/home/aigis/.venv/bin:$PATH"

# Default: drop into a shell so developers can run make, explore, debug, etc.
CMD ["/bin/bash"]
