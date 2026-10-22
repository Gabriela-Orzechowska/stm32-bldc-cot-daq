#include "common.h"
#include "tcp.h"
#include "lwip.h"
#include "lwip/tcp.h"
#include "lwip/mem.h"

#define SERVER_PORT 5000

static server_recv_callback s_server_recv_callback = NULL;
static server_callback s_server_connect_callback = NULL;

static struct tcp_pcb* s_client_pcb = NULL;

static err_t _server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static void _server_error(void *arg, err_t err);
static err_t _server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void _server_process_data(const void* buffer, size_t size);

void server_set_recv_callback(server_recv_callback cb) {
	s_server_recv_callback = cb;
}

void server_set_on_connect_callback(server_callback cb) {
	s_server_connect_callback = cb;
}

bool server_send_data(const void* data, size_t size) {
	if (!s_client_pcb) return false;
	if (!data || !size) return false;

	const char* p = (const char *)data;
	while (size > TCP_SND_BUF) {
		err_t ret = tcp_write(s_client_pcb, p, TCP_SND_BUF, TCP_WRITE_FLAG_COPY);
		if (ret != ERR_OK) return false;

		ret = tcp_output(s_client_pcb);
		if (ret != ERR_OK) return false;

		p += TCP_SND_BUF;
		size -= TCP_SND_BUF;
	}

	err_t ret = tcp_write(s_client_pcb, p, size, TCP_WRITE_FLAG_COPY);
	if (ret != ERR_OK) return false;

	return tcp_output(s_client_pcb) == ERR_OK;
}

bool server_init(void) {
	struct tcp_pcb *tpcb = tcp_new();
	if (!tpcb) return false;

	err_t ret = tcp_bind(tpcb, IP_ADDR_ANY, SERVER_PORT);

	if (ret != ERR_OK) {
		tcp_abort(tpcb);
		return false;
	}

	tpcb = tcp_listen(tpcb);
	if (!tpcb) return false;

	tcp_accept(tpcb, _server_accept);
	return true;
}


static err_t _server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
	(void)arg;

	if (err != ERR_OK || !newpcb) return ERR_VAL;

	s_client_pcb = newpcb;

	tcp_recv(newpcb, _server_recv);
	tcp_err(newpcb, _server_error);

	if (s_server_connect_callback) s_server_connect_callback();

	return ERR_OK;
}

static err_t _server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	if (!p) {
		tcp_close(tpcb);

		if (tpcb == s_client_pcb)
			s_client_pcb = NULL;
		return ERR_OK;
	}

	if (err != ERR_OK) {
		pbuf_free(p);
		return err;
	}

	tcp_recved(tpcb, p->tot_len);

	for(struct pbuf* q = p; q; q = q->next) {
		_server_process_data(q->payload, q->len);
	}

	pbuf_free(p);

	return ERR_OK;
}

static void _server_error(void *arg, err_t err)
{
    (void)arg;
    (void)err;

    s_client_pcb = NULL;
}

static void _server_process_data(const void* data, size_t size) {
	if (s_server_recv_callback)
		s_server_recv_callback(data, size);
}
