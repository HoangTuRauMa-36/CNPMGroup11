FROM gcc:latest

WORKDIR /app

COPY src/ .

RUN g++ main.cpp Library.cpp -o app

CMD ["./app"]
