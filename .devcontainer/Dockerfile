# Start from a base Debian or Ubuntu image since they typically have easy access to Gforth via package managers.
FROM debian:bullseye-slim

USER root

# Avoid prompts from apt.
ENV DEBIAN_FRONTEND=noninteractive

# This sets up a basic environment and installs Gforth.
RUN apt-get update && apt-get install -y \
    git gcc make file curl \
    # You can include other necessary tools here, depending on your development needs.
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp
RUN git clone  https://github.com/uho/Elan

ENV LD_LIBRARY_PATH /usr/local/lib
WORKDIR /tmp/Elan/mimir-3.1
RUN ./configure
RUN make
RUN make install

# Fix installation
# RUN cp /usr/local/share/dcg/include/* /usr/local/include
# RUN cp /usr/local/lib 

WORKDIR /tmp/Elan/elan-1.11
RUN ./configure 
RUN make 
RUN make install

RUN rm -rf /tmp/Elan

# Fetch and install VS Code Elan extension
#RUN curl -L -o /root/elan-0.1.0.vsix https://github.com/uho/Elan/raw/main/.vscode/extensions/elan/elan-0.1.0.vsix
#RUN mkdir -p /root/.vscode-remote/extensions/ulrich_hoffmann.elan-0.1.0 
#WORKDIR /root/.vscode-remote/extensions/ulrich_hoffmann.elan-0.1.0
#RUN unzip /root/elan-0.1.0.vsix && mv extension.vsixmanifest .vsixmanifest && mv extension/* . && rm -rf "./[Content_Types].xml" extension

# Set the default shell to bash instead of sh
ENV SHELL /bin/bash
