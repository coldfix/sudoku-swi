FROM alpine:3.12.3 as build

RUN apk add --no-cache gcc g++ boost-dev make

WORKDIR /sudoku
COPY ./generator /sudoku
RUN make

FROM alpine:3.12.3
RUN apk add --no-cache tini lighttpd fcgi php7 php7-cgi php7-json
RUN adduser -D -H -h /sudoku -u 9001 sudoku

COPY ./html                         /sudoku/html
COPY --from=build /sudoku/sudoku    /sudoku/html/sudoku
COPY ./lighttpd.conf                /sudoku/lighttpd.conf

WORKDIR /sudoku
EXPOSE 3000
USER sudoku

ENTRYPOINT ["/sbin/tini", "--"]
CMD ["lighttpd", "-Df", "/sudoku/lighttpd.conf"]
