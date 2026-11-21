FROM alpine:3.24@sha256:f59abe656b692d901e184be3445e084786602108599d2a51d229c0f5a48c71bf AS build

RUN apk add --no-cache gcc g++ boost-dev make

WORKDIR /sudoku
COPY ./generator /sudoku
RUN make

FROM alpine:3.24@sha256:f59abe656b692d901e184be3445e084786602108599d2a51d229c0f5a48c71bf
RUN apk upgrade --no-cache && \
    apk add --no-cache lighttpd fcgi php84 php84-cgi

RUN adduser -D -H -h /sudoku -u 9001 sudoku

COPY ./html                         /sudoku/html
COPY --from=build /sudoku/sudoku    /sudoku/html/sudoku
COPY ./lighttpd.conf                /sudoku/lighttpd.conf

WORKDIR /sudoku
EXPOSE 3000
USER sudoku

CMD ["lighttpd", "-Df", "/sudoku/lighttpd.conf"]
