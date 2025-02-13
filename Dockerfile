FROM ubuntu:22.04

# Установка зависимостей
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    libgl1-mesa-dev \
    libxkbcommon-x11-dev \
    qt6-base-dev \
    qt6-tools-dev-tools

# Установка переменных окружения для Qt
ENV PATH="/usr/lib/qt6/bin:${PATH}"

# Создание рабочей директории
WORKDIR /app

# Копирование проекта внутрь контейнера (опционально)
COPY . /app

