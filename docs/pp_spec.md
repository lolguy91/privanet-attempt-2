# Privanet Protocol Specification
# WARNING: This initial version has been made with the use of ChatGPT, correction may be required
https://chat.openai.com/share/c497cc94-a958-47a9-9b7c-5ea298976db2
## 1. Introduction

The Privanet Protocol (PP) facilitates communication in a serverless mesh network, allowing nodes to interact directly without relying on centralized servers.

## 2. Protocol Overview

Privanet operates on existing network protocols, utilizing IP and Bluetooth for node communication. The protocol includes mechanisms for node discovery, identity exchange, and the transmission of 512-byte unencrypted data payloads.

## 3. Packet Structure

All communication in Privanet is packet-based. Each packet comprises a header and data. The header includes a type identifier to distinguish different packet types.

- **Packet Header**
  - 1 byte: Packet Type
  - N bytes: Data (variable size)

## 4. Packet Types

- **PT_BEAM (0x01):** Beam Packet
  - Data: BEAM_MAGICNUM (8 bytes)
  - Sent during node exploration for discovery.

- **PT_IDENT (0x02):** Identification Packet
  - Data:
    - BEAM_MAGICNUM (8 bytes) for verification
    - Sender's UUID (8 bytes)
    - Sender's Hostname (up to 255 characters)
  - Sent as a response to PT_BEAM during exploration.

- **PT_RESP_IDENT (0x03):** Response Identification Packet
  - Data:
    - BEAM_MAGICNUM (8 bytes) for verification
    - Sender's UUID (8 bytes)
    - Sender's Hostname (up to 255 characters)
  - Sent as a response to PT_IDENT during exploration.

- **PT_DATA (0x04):** Data Packet
  - Data: Unencrypted payload (512 bytes)
  - Used for transmitting unencrypted data between nodes.

## 5. Node Discovery

During exploration, nodes send PT_BEAM packets for discovery. If a node receives a PT_BEAM packet, it responds with PT_IDENT. The sender then replies with PT_RESP_IDENT, establishing mutual identification.

## 6. Data Transmission

Nodes exchange unencrypted data using PT_DATA packets, with a fixed payload size of 512 bytes.

## 7. Session Management

Each node has a unique session ID based on the network protocol and port, used for addressing nodes in the mesh network.

## 8. Additional Features (To Be Decided)

- **Find by UUID Feature:**
  - Reserved space for a feature allowing nodes to find each other based on UUID.

- **Network Crawling:**
  - Reserved space for a mechanism to crawl the network and discover available nodes.

- **Contact List Structure:**
  - Reserved space for detailing the structure of a contact list, considering factors such as online status, last contact time, etc.

## 9. Security Considerations

Privanet currently lacks encryption mechanisms for data in transit. Caution is advised when using Privanet in untrusted network environments.

## 10. Future Considerations

Future iterations of Privanet may include additional features such as message acknowledgment, enhanced security measures, and support for additional network protocols.
