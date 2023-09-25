# First Stage
FROM ubuntu:18.04 AS builder

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
	    curl \
        libglib2.0-0 \
        libatlas-base-dev \
        zlib1g-dev \
        libwxgtk3.0-dev

COPY . ift
ENV NEWIFT_DIR /ift
WORKDIR /ift
RUN make -j4 
RUN make -j4 -Cdemo/VISVA/.

WORKDIR /.
RUN mkdir /libraries/ && \
    cp /ift/bin/* /libraries/ && \
    cp /ift/lib/libift.a /libraries/ && \
    rm -rf /ift 

# Second Stage
FROM ubuntu:18.04

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
	    curl \
        libglib2.0-0 \
        libatlas-base-dev \
        zlib1g-dev \
        libwxgtk3.0-dev

COPY --from=builder /libraries/ /libraries/
RUN mkdir -p /ift/lib && ln -s /libraries/libift.a /ift/lib/
CMD ["/libraries/visva"]
