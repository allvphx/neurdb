apt-get update && apt-get install -y --no-install-recommends \
    openjdk-17-jdk-headless \
    libjna-java \
    git curl ca-certificates \
    openssh-client rsync \
    bash jq vim less \
    && rm -rf /var/lib/apt/lists/*

curl -fsSL https://raw.githubusercontent.com/technomancy/leiningen/stable/bin/lein \
      -o /usr/local/bin/lein \
    && chmod +x /usr/local/bin/lein \
    && lein --version
