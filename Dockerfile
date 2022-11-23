FROM alpine:3.17.0 as build

RUN apk add --no-cache gcc g++ boost-dev make

WORKDIR /sudoku
COPY ./generator /sudoku
RUN make

FROM alpine:3.17.0
RUN apk add --no-cache lighttpd fcgi php7 php7-cgi php7-json
RUN adduser -D -H -h /sudoku -u 9001 sudoku

COPY ./html                         /sudoku/html
COPY --from=build /sudoku/sudoku    /sudoku/html/sudoku
COPY ./lighttpd.conf                /sudoku/lighttpd.conf

WORKDIR /sudoku
EXPOSE 3000
USER sudoku

CMD ["lighttpd", "-Df", "/sudoku/lighttpd.conf"]
