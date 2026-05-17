# Visual Studio Build Environment for Mednafen
FROM mcr.microsoft.com/windows/servercore:ltsc2022

# Install Chocolatey for package management
RUN powershell -NoProfile -ExecutionPolicy Bypass -Command \
    [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; \
    iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# Install Visual Studio Build Tools 2022 and CMake
RUN choco install -y visualstudio2022buildtools cmake --installargs 'ADD_CMAKE=1'

# Set up environment variables for VS tools
ENV VSINSTALLDIR="C:\\Program Files (x86)\\Microsoft Visual Studio\\2022\\BuildTools"
ENV PATH="${VSINSTALLDIR}\\MSBuild\\Current\\Bin;${VSINSTALLDIR}\\Common7\\IDE;${PATH}"

# Default workdir
WORKDIR /workspace

# Copy source (if using devcontainer, this is handled by bind mount)
# COPY . /workspace

# Default command
CMD ["cmd.exe"]
