FROM debian:stable AS builder

WORKDIR /src

RUN apt-get update \
    && apt-get install -y --no-install-recommends build-essential cmake git \
    && rm -rf /var/lib/apt/lists/*

COPY . .

RUN cmake -S /src -B /src/build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /src/build --config Release -j"$(nproc)"

FROM debian:stable-slim

WORKDIR /app

RUN apt-get update \
    && apt-get install -y --no-install-recommends ca-certificates \
    && rm -rf /var/lib/apt/lists/*

EXPOSE 25565 20220

COPY --from=builder /src/build/minecraftspeedproxy /usr/local/bin/minecraftspeedproxy

ENTRYPOINT ["/usr/local/bin/minecraftspeedproxy"]
