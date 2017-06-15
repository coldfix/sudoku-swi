FROM alpine:3.6

COPY . /sudoku
WORKDIR /sudoku

ARG build_deps="make gcc g++ boost-dev"

RUN apk update && \
    apk add -u tini lighttpd fcgi php7 php7-cgi $build_deps && \
    make && \
    apk del $build_deps && \
    rm -rf generator /var/cache/apk/*

EXPOSE 3000

ENTRYPOINT ["/sbin/tini", "--"]
CMD ["lighttpd", "-Df", "/sudoku/lighttpd.conf"]
