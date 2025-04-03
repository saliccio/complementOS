#pragma once

void pit_init();

void pit_wait_ms(u64_ct ms);

void pit_isr_handler();