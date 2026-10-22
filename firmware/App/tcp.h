#pragma once

typedef void(*server_recv_callback)(const void*, size_t);
typedef void(*server_callback)(void);

bool server_init(void);
bool server_send_data(const void* data, size_t size);

void server_set_recv_callback(server_recv_callback cb);
void server_set_on_connect_callback(server_callback cb);

