FROM osrf/ros:jazzy-desktop-full

ARG USERNAME=ubuntu

# root
RUN apt-get update && apt-get install -y \
    sudo \
    python3-full \
    python3-venv \
    python3-pip \
    ros-jazzy-cartographer \
    ros-jazzy-cartographer-ros \
    ros-jazzy-cartographer-rviz \
    ros-jazzy-imu-tools \
    ros-jazzy-navigation2 \
    ros-jazzy-nav2-bringup \
    ros-jazzy-joint-state-publisher-gui \
    neovim \
    tree \
 && rm -rf /var/lib/apt/lists/*

RUN echo "ubuntu ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/ubuntu \
 && chmod 0440 /etc/sudoers.d/ubuntu

# user
USER ${USERNAME}
WORKDIR /home/${USERNAME}/project

RUN python3 -m venv /home/${USERNAME}/pio-venv \
 && /home/${USERNAME}/pio-venv/bin/pip install -U pip \
 && /home/${USERNAME}/pio-venv/bin/pip install platformio

# bashrc config
RUN echo "source /opt/ros/jazzy/setup.bash" >> ~/.bashrc \
 && echo "alias python='python3'" >> ~/.bashrc \
 && echo "alias vim='nvim'" >> ~/.bashrc \
 && echo "alias platformio='$HOME/pio-venv/bin/pio'" >> ~/.bashrc \
 && echo "export __EGL_VENDOR_LIBRARY_FILENAMES=/usr/share/glvnd/egl_vendor.d/10_nvidia.json" >> ~/.bashrc \
 && echo "export __GLX_VENDOR_LIBRARY_NAME=nvidia" >> ~/.bashrc

# create platformio's venv penv
RUN rm -rf ~/.platformio/penv \
 && python3 -m venv ~/.platformio/penv

# install packages for mros build
RUN ~/.platformio/penv/bin/pip install \
  catkin-pkg \
  empy==3.3.4 \
  lark-parser \
  pyyaml \
  importlib-resources \
  markupsafe==2.0.1

# gz plugin
RUN sudo sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list' \
 && sudo curl https://packages.osrfoundation.org/gazebo.gpg --output /usr/share/keyrings/pkgs-osrf-archive-keyring.gpg \
 && echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/pkgs-osrf-archive-keyring.gpg] http://packages.osrfoundation.org/gazebo/ubuntu-stable $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/gazebo-stable.list > /dev/null \
 && apt-cache search libgz-tools \
 && sudo apt-get update \
 && sudo apt-get -y install libgz-tools2-dev \
 && sudo apt -y upgrade \
 && sudo rm -rf /var/lib/apt/lists/*

