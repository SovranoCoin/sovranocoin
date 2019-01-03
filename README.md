SovranoCoin (SVR) integration/staging repository
======================================

SovranoCoin is a cutting edge cryptocurrency, with many features not available in most other cryptocurrencies.
- Anonymized transactions using zerocoin technology.
- Fast transactions featuring guaranteed zero confirmation transactions named _SwiftX_.
- Decentralized blockchain voting providing for consensus based advancement of the current Masternode
  technology used to secure the network and provide the above features, each Masternode is secured
  with a collateral of 1000 SVR.


Quick installation of the SovranoCoin daemon under linux.

Installation of libraries (using root user):

    add-apt-repository ppa:bitcoin/bitcoin -y
    apt-get update
    apt-get install -y build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils
    apt-get install -y libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev
    apt-get install -y libdb4.8-dev libdb4.8++-dev

Cloning the repository and compiling (use any user with the sudo group):

    cd
    git clone https://github.com/SovranoCoin/sovranocoin.git
    cd sovranocoin
    ./autogen.sh
    ./configure
    sudo make install
    cd src
    sudo strip sovranocoind
    sudo strip sovranocoin-cli
    sudo strip sovranocoin-tx
    cd ..

Running the daemon:

    sovranocoind 

Stopping the daemon:

    sovranocoin-cli stop

Demon status:

    sovranocoin-cli getinfo
    sovranocoin-cli mnsync status



More information at [SovranoCoin.com](http://www.sovranocoin.com)


### Coin Specs
<table>
<tr><td>Algo</td><td>Quark</td></tr>
<tr><td>Block Time</td><td>90 Seconds</td></tr>
<tr><td>Block Split</td><td>80% MN / 20% Staking</td></tr>
<tr><td>Difficulty Retargeting</td><td>Every Block</td></tr>
<tr><td>Max Coin Supply</td><td>21.000.000 SVR</td></tr>
<tr><td>Premine</td><td>110.000 SVR</td></tr>
<tr><td>P2P port</td><td>6610</td></tr>
<tr><td>RPC port</td><td>6611</td></tr>
</table>