
            //     // u8 payload[250];
            //     // if (this->peer_count > 0)
            //     // {
            //     //     LOG_DEBUG("Preparing message");
            //     //     payload[0] = 5; // Length of the message (Hello)
            //     //     strcpy((char *)payload + 1, "Hello");
            //     // }
            //     // else
            //     // {
            //     //     LOG_DEBUG("No peers registered");
            //     // }

            //     // for (u8 peer_index = 0; peer_index < this->peer_count; peer_index++)
            //     // {
            //     //     u8 *peer_mac = this->peer_list[peer_index];

            //     //     LOG_DEBUG("Sending message to peer");
            //     //     esp_now_send(peer_mac, (u8 *)&payload, sizeof(payload));

            //     //     LOG_DEBUG("Message sent");
            //     // }