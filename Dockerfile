FROM ghcr.io/3-24/llvm:13

LABEL org.opencontainers.image.source=https://github.com/3-24/function-info-llvm

RUN apt-get update && \
    apt-get install -y make file

# Copy gllvm from ghcr.io/3-24/gllvm:latest
COPY --from=ghcr.io/3-24/gllvm:latest /go/bin/* /usr/local/bin/

COPY . /function-info
WORKDIR /function-info

RUN make
