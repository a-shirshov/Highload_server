FROM gcc:latest as build

WORKDIR /app
RUN apt-get update && apt-get install -y \
    cmake

COPY . .

RUN mkdir -p var/www/html && cp -r httptest var/www/html

WORKDIR /app/build
RUN cmake .. && make
EXPOSE 80

CMD ["/app/build/main"]