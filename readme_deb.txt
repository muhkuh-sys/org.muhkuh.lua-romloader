1) Checkout to muhkuh-2.1.2
  git clone --recursive https://github.com/muhkuh-sys/muhkuh.git muhkuh-2.1.2
2) Make orig archive
  tar --create --file muhkuh_2.1.2.orig.tar.gz --gzip muhkuh-2.1.2
3) Make debian archive
  tar --create --directory muhkuh-2.1.2/installer --file muhkuh_2.1.2-1.debian.tar.gz --gzip debian
