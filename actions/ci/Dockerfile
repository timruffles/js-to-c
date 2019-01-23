FROM silkeh/clang:7

ENV NODE_VERSION v11.6.0
ENV PATH="$PATH:./node_modules/.bin"

ADD https://nodejs.org/dist/${NODE_VERSION}/node-${NODE_VERSION}-linux-x64.tar.xz /root/node.tar.xz

RUN tar -xJf /root/node.tar.xz -C /usr/local/lib/

RUN ln -s /usr/local/lib/node-${NODE_VERSION}-linux-x64/bin/node /usr/local/bin/node && \
  ln -s /usr/local/lib/node-${NODE_VERSION}-linux-x64/bin/npm /usr/local/bin/npm

CMD "/root/code/scripts/ci"
