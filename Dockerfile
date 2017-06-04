# Only in 3.3: php php-cgi
FROM alpine:3.3

COPY . /sudoku
WORKDIR /sudoku

# 3.3 has no dumb-init nor tini, so we we have to download the binary manually.
# For that we need openssl, see: https://github.com/Yelp/dumb-init/issues/73.
# NOTE: need gcc/g++ runtime libs, so not uninstalling!
ENV build_deps="make openssl boost-dev"

RUN apk update && \
    apk add -u lighttpd fcgi php php-cgi gcc g++ $build_deps && \
    wget -O /usr/local/bin/dumb-init https://github.com/Yelp/dumb-init/releases/download/v1.2.0/dumb-init_1.2.0_amd64 && \
    chmod +x /usr/local/bin/dumb-init && \
    make && \
    apk del $build_deps && \
    rm -rf generator

EXPOSE 3000

ENTRYPOINT ["/usr/local/bin/dumb-init", "--"]
CMD ["lighttpd", "-Df", "/sudoku/docker/lighttpd.conf"]
