### Created by Dr. Benjamin Richards

### Download base image from cern repo on docker hub
FROM toolframework/centos7

### Run the following commands as super user (root):
USER root


RUN git clone https://github.com/ToolFramework/ToolFrameworkCore.git \opt\ToolFrameworkCore \
    && cd \opt\ToolFrameworkCore \
    && make

### Open terminal
CMD ["/bin/bash"]