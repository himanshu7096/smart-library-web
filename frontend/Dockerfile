FROM gcc:latest

WORKDIR /app

COPY backend /app/backend
COPY frontend /app/frontend

WORKDIR /app/backend

RUN g++ -std=c++17 server.cpp -o server -pthread

ENV FRONTEND_DIR=/app/frontend

CMD ["./server"]