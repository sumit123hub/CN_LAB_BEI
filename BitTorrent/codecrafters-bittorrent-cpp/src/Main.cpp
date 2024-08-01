#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "lib/nlohmann/json.hpp"

#include <cpr/cpr.h>
#include "bencode/decode.hpp"
#include "bencode/encode.hpp"
#include "metainfo/metainfo.hpp"
#include "client/client.hpp"
#include "client/connection.hpp"

using json = nlohmann::json;


int decode_command(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }
    std::string encoded_value = argv[2];

    try
    {
        Decode decode = Decode();
        std::cout << decode.decode_bencoded_value(encoded_value).dump() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}


int info_command(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " info <torrent_file>" << std::endl;
        return 1;
    }

    std::string torrent_file = argv[2];

    try
    {
        MetaInfo meta_info = MetaInfo(torrent_file);
        std::cout << "Tracker URL: " << meta_info.get_announceURL() << std::endl;
        std::cout << "Length: " << meta_info.get_file_size() << std::endl;
        std::cout << "Info Hash: " << meta_info.get_info_hash() << std::endl;
        std::cout << "Piece Length: " << meta_info.get_piece_length() << std::endl;
        std::cout << "Piece Hashes: " << std::endl;
        for (auto &hash : meta_info.get_pieces_hash())
        {
            std::cout << hash << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}


int peers_command(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " peers <torrent_file>" << std::endl;
        return 1;
    }

    std::string torrent_file = argv[2];

    try
    {
        MetaInfo metaInfo = MetaInfo(torrent_file);
        Client cli = Client();
        std::vector<std::string> peers = cli.discover_peers(metaInfo);

        for (auto &peer : peers)
        {
            std::cout << peer << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}


int handshake_command(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cerr << "Usage: " << argv[0] << " handshake <torrent_file> <peer_ip>:<peer_port>" << std::endl;
        return 1;
    }

    std::string torrent_file = argv[2];

    try
    {
        std::string address = argv[3];
        std::string peer_ip = address.substr(0, address.find(":"));
        std::string peer_port = address.substr(address.find(":") + 1);

        MetaInfo metaInfo = MetaInfo(torrent_file);
        Client cli = Client();
        Connection peerConnection = Connection(peer_ip, peer_port);
        std::string peerID = cli.get_peer_id(metaInfo, peerConnection);
        std::cout << "Peer ID: " << peerID << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}

int download_piece_command(int argc, char *argv[])
{
    if (argc < 6)
    {
        std::cerr << "Usage: " << argv[0] << " download_piece -o <output_file> <torrent file> <piece_index>" << std::endl;
        return 1;
    }

    std::string output_file = argv[3];
    std::string torrent_file = argv[4];
    size_t piece_index = std::stoul(argv[5]);

    try
    {
        MetaInfo metaInfo = MetaInfo(torrent_file);
        Client cli = Client();
        cli.download_piece(metaInfo, output_file, piece_index);
        std::cout << "Downloaded piece " << piece_index << " to " << output_file << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}


int download_file_command(int argc, char *argv[])
{
    if (argc < 5)
    {
        std::cerr << "Usage: " << argv[0] << " download -o <output_file> <torrent file>" << std::endl;
        return 1;
    }

    std::string output_file = argv[3];
    std::string torrent_file = argv[4];

    try
    {
        MetaInfo metaInfo = MetaInfo(torrent_file);
        Client cli = Client();
        cli.download_file(metaInfo, output_file);
        std::cout << "Downloaded " << torrent_file << " to " << output_file << "." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc < 2)
    {
        std::cerr << "Usage: \t " << argv[0] << " decode <encoded_value>" << std::endl;
        std::cerr << "\t " << argv[0] << " info <torrent file>" << std::endl;
        std::cerr << "\t " << argv[0] << " peers <torrent file>" << std::endl;
        std::cerr << "\t " << argv[0] << " handshake <torrent file> <peer_ip>:<peer_port>" << std::endl;
        std::cerr << "\t " << argv[0] << " download_piece -o <output_file> <torrent file> <piece_index>" << std::endl;
        std::cerr << "\t " << argv[0] << " download -o <output_file> <torrent file>" << std::endl;
        return 1;
    }

    std::string command = argv[1];

    if (command == "decode")
    {
        return decode_command(argc, argv);
    }
    else if (command == "info")
    {
        return info_command(argc, argv);
    }
    else if (command == "peers")
    {
        return peers_command(argc, argv);
    }
    else if (command == "handshake")
    {
        return handshake_command(argc, argv);
    }
    else if (command == "download_piece")
    {
        return download_piece_command(argc, argv);
    }
    else if (command == "download")
    {
        return download_file_command(argc, argv);
    }
    else
    {
        std::cerr << "unknown command: " << command << std::endl;
        return 1;
    }
}