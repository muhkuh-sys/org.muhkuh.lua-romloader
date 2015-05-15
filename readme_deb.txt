1) Checkout to muhkuh-2.1.0
  git clone --recursive https://github.com/muhkuh-sys/muhkuh.git muhkuh-2.1.0
2) Make orig archive
  tar --create --file muhkuh_2.1.0.orig.tar.gz --gzip muhkuh-2.1.0
3) Make debian archive
  cd muhkuh-2.1.0 && tar --create --file muhkuh_2.1.0-1.debian.tar.gz --gzip debian
