FROM mcr.microsoft.com/devcontainers/base:ubuntu

RUN apt-get update && apt-get install -y build-essential libtool autoconf unzip wget

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="3.25.0"

COPY ./reinstall-cmake.sh /tmp/
RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
        chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
    fi \
    && rm -f /tmp/reinstall-cmake.sh

RUN apt-get install -y ninja-build
RUN curl -LsSf https://astral.sh/uv/install.sh | sh

# 设置环境变量
ENV ANDROID_NDK_VERSION r27c
ENV ANDROID_NDK_HOME /opt/android-ndk
ENV ANDROID_NDK /opt/android-ndk

# 下载并安装Android NDK
RUN wget -q https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux.zip && \
    unzip -q android-ndk-${ANDROID_NDK_VERSION}-linux.zip -d /opt && \
    mv /opt/android-ndk-${ANDROID_NDK_VERSION} ${ANDROID_NDK_HOME} && \
    rm android-ndk-${ANDROID_NDK_VERSION}-linux.zip

# 设置环境变量
ENV PATH ${PATH}:${ANDROID_NDK_HOME}
