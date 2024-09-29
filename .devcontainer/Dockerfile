# Start from a base Debian
FROM debian:bullseye-slim

USER root

# Avoid prompts from apt.
ENV DEBIAN_FRONTEND=noninteractive

# This sets up a basic environment
RUN apt-get update && apt-get install -y \
    git gcc make file curl \
    # You can include other necessary tools here, depending on your development needs.
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp
RUN git clone https://github.com/uho/Elan

WORKDIR /tmp/Elan/mimir-3.1
RUN ./configure
RUN make
RUN make install

# let dcg find its libraries
ENV LD_LIBRARY_PATH /usr/local/lib

WORKDIR /tmp/Elan/elan-1.11
RUN ./configure 
RUN make 
RUN make install

RUN rm -rf /tmp/Elan

# Set the default shell to bash instead of sh
ENV SHELL /bin/bash
