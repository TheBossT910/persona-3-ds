# Persona 3 Dual - Developer Environment
#
# Based on the official BlocksDS image
# Adds build dependencies

# ==========================================
# Base stage - only for GitHub ACTIONS
# GitHub Actions targets this stage
# ==========================================

# Use the dev-latest because it contains the source code of blocksds, so we can step in on the debugger
FROM skylyrac/blocksds:dev-latest AS base

LABEL maintainer="The P3D Project"
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
    && git lfs install --system \
    && rm -rf /var/lib/apt/lists/*

# Python virtual environment for CI (GitHub Actions runs as root)
# The Makefile calls $(HOME)/.venv/bin/python3 directly, so this only needs
# to exist at /root/.venv
# No need to touch PATH.
COPY tools/requirements.txt /tmp/requirements.txt
RUN python3 -m venv /root/.venv \
    && /root/.venv/bin/pip install --no-cache-dir --upgrade pip \
    && /root/.venv/bin/pip install --no-cache-dir -r /tmp/requirements.txt

# ==========================================
# Dev Container stage - only for developers
# GitHub Actions targets base only
# ==========================================
FROM base AS devcontainer

# Developers specific packages
# sudo          – just for the case aigis need root access
# gdb-multiarch – debugger
RUN apt-get update && apt-get install -y --no-install-recommends \
    sudo \
    gdb-multiarch \
    && rm -rf /var/lib/apt/lists/*

# Add aigis user so we don't run as root on dev container
# Also gives access to opt/wonderful (Wonderful Toolchain)
RUN useradd -m aigis \
    && chown -R aigis:aigis /opt/wonderful

# Give sudo access to aigis
RUN echo "aigis ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/aigis \
    && chmod 0440 /etc/sudoers.d/aigis \
    && visudo -c -f /etc/sudoers.d/aigis

USER aigis

# venv for aigis (same requirements.txt as CI, resolved via $(HOME) in Makefile)
RUN python3 -m venv "$HOME/.venv" \
    && "$HOME/.venv/bin/pip" install --no-cache-dir --upgrade pip \
    && "$HOME/.venv/bin/pip" install --no-cache-dir -r /tmp/requirements.txt

# Default: drop into a shell so developers can run make, explore, debug, etc.
CMD ["/bin/bash"]
