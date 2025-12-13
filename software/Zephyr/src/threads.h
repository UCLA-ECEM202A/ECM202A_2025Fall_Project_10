#ifndef THREADS_H
#define THREADS_H

void uart_rx_tx_thread(void *, void *, void *);
void process_messages_thread(void *, void *, void *);
// void user_input_thread(void *, void *, void *);

#endif
