ARG MUID=1000
ARG MGID=1000
ARG USERNAME=developer

FROM rocm/dev-ubuntu-22.04:6.2-complete
ENV DEBIAN_FRONTEND=noninteractive

RUN apt -y autoremove
RUN apt-get update && apt-get install -y vim zsh zsh-doc tmux git less man man-db mlocate && \
apt-get install -y cmake cmake-curses-gui clangd bash-completion && \
apt-get install -y silversearcher-ag keychain htop rcm && \
apt-get install -y libbz2-dev liblzma-dev libssl-dev libreadline-dev libzstd-dev && \
pip3 install pybind11 websockets CppHeaderParser

RUN apt-get -y install arcanist rocm-debug-agent && updatedb
RUN apt-get install -y locales && locale-gen en_US.UTF-8 

ARG MUID
ARG MGID
ARG USERNAME

RUN addgroup --gid ${MGID} ${USERNAME}
RUN useradd -d /home/${USERNAME} -g ${MGID} --no-create-home -u ${MUID} --shell /usr/bin/bash ${USERNAME}
RUN adduser ${USERNAME} sudo
RUN adduser ${USERNAME} video
RUN adduser ${USERNAME} render
RUN mkdir /home/${USERNAME} /var/${USERNAME}
RUN chown ${USERNAME}:${USERNAME} /home/${USERNAME} /var/${USERNAME}

RUN sed -i~ -e 's/%sudo\tALL=(ALL:ALL) ALL/%sudo\tALL=(ALL:ALL) NOPASSWD:ALL/g' /etc/sudoers

USER ${USERNAME}
WORKDIR /home/${USERNAME}
ENV HOME=/home/${USERNAME}

CMD tmux new-session -d && tail -f /dev/null

