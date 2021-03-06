/*
    Interrupts are handled via the Interrupt Descriptor Table (IDT).
    The IDT describes a handler for each interrupt. The interrupts are numbered
   (0 - 255) and the handler for interrupt i is defined at the ith position in
   the table. There are three different kinds of handlers for interrupts: Task
   handler, Interrupt handler, Trap handler

    The only difference between an interrupt handler and a trap handler is that
   the interrupt handler disables interrupts, which means you cannot get an
   interrupt while at the same time handling an interrupt.
*/

#include "idt.h"

#include "console.h"
#include "inttypes.h"
#include "io.h"
#include "processus.h"
#include "segment.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

/* Nombre d'interruptions ou encore nombre d'entrée dans l'IDT */
#define NUM_IDT_ENTRIES (256)
#define IDT_BASE (0x1000)

static idt_ptr_t idt_ptr;
static uint32_t uptime = 0;

/**
 * Initialisation de la table des vecteurs d’interruption
 *
 * On appelera simplement init_idt_entry(32, traitant_IT_32);
 * pour initialiser la table des vecteurs d'interruption.
 *
 * Un traitant est un pointeur de fonction.
 * "Instead of using bytes (or unsigned integers) use packed structures to make
 * the code more readable." See idt.h.
 * 
 * Do not call outside of init_idt()
 * 
 * @pre idt_ptr.base is set. KERNEL_CS is defined.
 * 
 * @param interrupt_index index de l'interruption dans IDT
 * @param interrupt_handler assembly function that handle the interruption or ISR wrapper
 */
static inline void init_idt_entry(int32_t interrupt_index,
                                  void (*interrupt_handler)(void)) {
    idt_entry_t *interrupt = (idt_entry_t *)idt_ptr.base + interrupt_index;
    uint32_t handler = (uint32_t)interrupt_handler;

    interrupt->base_lo = handler & 0x0000FFFF;
    interrupt->base_hi = (uint16_t)(handler >> 16) & 0x0000FFFF;
    interrupt->segment_selector = KERNEL_CS; /* kernel code segment offset */
    interrupt->flags = 0x8E00;               /* interrupt gate: used to transfer control of
                                    execution across segments */
}

/**
 * Initialisation de la table des vecteurs d'interruption
 * Appelée dans kernel_start()
 * 
 * @pre IDT_BASE, NUM_IDT_ENTRIES and init_idt_entry() are defined. interrupt_handler assembly functions are defined in idt.h
 */
inline void init_idt(void) {
    /* Initialisation de l'idt */
    idt_ptr.base = (uint16_t)IDT_BASE;
    idt_ptr.limit = NUM_IDT_ENTRIES * sizeof(idt_entry_t) - 1;

    /* Construction de la table */
    init_idt_entry(32, traitant_IT_32);  // 32, irq0
}

/**
 * Renvoie l'heure
 *
 * @param hour buffer contenant l'heure
 * @param seconds //
 * @return affiche l'heure hh:mm:ss
 */
static inline void get_hour(char *hour, uint32_t seconds) {
    sprintf(hour, "%02u:%02u:%02u", (seconds / 3600), ((seconds / 60) % 60), (seconds % 60));
}

/**
 * Renvoie l'uptime
 * 
 * @return uptime
 */ 
uint32_t get_uptime(void) {
    return uptime;
}

/**
 * ISR (Interrupt Service Routine)
 * Programmable Interval Timer
 * 
 * @pre pic_acknowledge() from io.c. console_write_hour() from console.c. CLOCK_FREQ and get_hour() are defined.
 * 
 * Called in traitant.S
 * 
 * TODO: point d'arrêt pour ragrder l'affichage de l'horloge
 */
void tic_PIT(void) {
    pic_acknowledge();

    /* 
     * When the variable is declared inside the function, it has local scope only to that function. 
     * To reduce the scope of a variable as much as possible is always good practice.
    */
    static uint32_t compteur = 0;
    compteur += 1;

    char hour[8] = "00:00:00";  // on laisse le compilateur déterminer la taille

    /* Affichage de l'horloge */
    if (compteur % CLOCKFREQ == 0) {
        get_hour(hour, (compteur / CLOCKFREQ));
        console_write_hour(hour);
        uptime++;
    }
    scheduler();
}

/** 
 * L’octet récupéré est un tableau de booléens tel que la valeur du bit N décrit l’état de 
 * l’IRQ N : 1 si l’IRQ est masquée, 0 si elle est autorisé. Il faut donc forcer la valeur 
 * du bit N à la valeur souhaitée (sans toucher les valeurs des autres bits) et envoyer ce 
 * masque sur le port de données.
 * 
 * @param num_IRQ numéro de l'IRQ (Interrupt Request)
 * @param masque masquage ou non de l'IRQ
 */
void masque_IRQ(uint32_t num_IRQ, bool masque) {
    (masque) ? set_IRQ_mask(num_IRQ) : clear_IRQ_mask(num_IRQ);
}