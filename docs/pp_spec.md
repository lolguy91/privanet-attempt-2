# Privanet Protocol Specification
## WARNING: This initial version has been made with the use of ChatGPT, correction may be required
### current version is **HR**(Human Revision) v0.00001
[ChatGPT convo found here](https://chat.openai.com/share/6d97b261-7769-4a95-bee4-0157a8ae8e07)

## 1. Overview
The **PP**(**P**rivanet **P**rotocol) facilitates communication in a decentralized mesh network.The whole network can be thought of as a big graph of nodes(devices) and connections. Each node has one or more connections, allowing them to exchange data.</br>
The speed of the data transfer can depend on:
 - Number of connections
 - Speed of the connections
 - Type of the connections

PP's current connection methods:
 - LAN
 - WiFi Aware(Only mobile chipsets support this tho)
 - Local-only hotspot
 - WiFi Direct

The protocol's current features:
 - Node discovery
 - Identity exchange
 - Data transmission.

Here are a few parameters:
 - Using port 56969 or 55556

## 2. Identification: UUIDs,Neighbour IDs and IP addresses
- **UUID**: a unique ID of a device that makes sending packets private, yet effective
- **Neighbour ID**: an index into a device's neighbour list, only used during handling, **NOT** included in any packets, if we ask you to, it is **our** fault and should be corrected
- **IP address**: you prob alredy know hat dat is, we do both v4 and v6, andonly used for during handling, same as the prev.


## 3. Packet Structure
All communication in Privanet is packet-based. Each packet comprises a header and a body.
### 3.0 Packet Header
The header is just a type identifier lol.
- **Packet Header**
  - 1 byte: Packet Type
  - N bytes: Body(variable size)

### 3.1 Packet Types
The Privanet protocol has `4` packet types
```c
#define PT_BEAM         0x00 //Beam Packet
#define PT_IDENT        0x01 //Identification Packet
#define PT_RESP_IDENT   0x02 //Response Identification Packet
#define PT_DATA         0x03 //Data Packet
```
### 3.2 Packet Body

- **PT_BEAM** 
  - Length (8 bytes)
  - Body Content: BEAM_MAGICNUM

- **PT_IDENT** 
  - Length (262 bytes)
  - Body Content:
    - BEAM_MAGICNUM (8 bytes) for verification
    - Sender's UUID (8 bytes)
    - Sender's Hostname (up to 256 characters)

- **PT_RESP_IDENT** 
  - Length (262 bytes)
  - Body Content:
    - BEAM_MAGICNUM (8 bytes) for verification
    - Sender's UUID (8 bytes)
    - Sender's Hostname (up to 256 characters)

- **PT_DATA** 
  - Length (516 bytes)
  - Body Content: 
    - Length (4 bytes)
    - Unencrypted payload (512 bytes)

## 4. Node Discovery
During exploration, the following things happen **in order**:
 1. Sender sends **PT_BEAM** packets to all possible neighbours.
 2. If a node receives a **PT_BEAM** packet, it responds with PT_IDENT.
 3. Sender then replies with **PT_RESP_IDENT**, establishing a connection.

## 5. Additional Features (To Be Decided)

- **Find by UUID Feature:**
  - Reserved space for a feature allowing nodes to find each other based on UUID.

- **Network Crawling:**
  - Reserved space for a mechanism to crawl the network and discover available nodes.