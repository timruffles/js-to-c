FROM node:11.10.1-stretch

COPY . /app

WORKDIR /app/compile-server
RUN npm install --production

WORKDIR /app
RUN make install

CMD ["/usr/bin/node", "/app/index.js"]
