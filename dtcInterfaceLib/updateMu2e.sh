if [[ ! -f DTC.node ]]; then
  source ./build.sh
fi
ssh mu2edaq01.dhcp "mkdir -p ~/dev/modules/DTC/server"
scp DTC.node mu2edaq01.dhcp:~/dev/modules/DTC/server
scp -r web/modules/* mu2edaq01.dhcp:~/dev/modules
scp web/serverbase.js mu2edaq01.dhcp:~/dev
scp web/client.html mu2edaq01.dhcp:~/dev
ssh mu2edaq01.dhcp "touch ~/dev/.devbroken"
