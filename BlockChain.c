#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

void initBlockchain(Blockchain *blockchain) {
    blockchain->length = 0;
}

int addBlock(Blockchain *blockchain) {
    if (blockchain->length >= BLOCKCHAIN_CAPACITY) {
        printf("Error: Blockchain capacity reached\n");
        return 0;
    }

    Block *block = &blockchain->chain[blockchain->length];
    block->index = blockchain->length;
    block->timestamp = (uint64_t)time(NULL);

    if (blockchain->length == 0) {
        strcpy(block->prev_hash, "0000000000000000000000000000000000000000000000000000000000000000");
    } else {
        Block *prev_block = &blockchain->chain[blockchain->length - 1];
        strncpy(block->prev_hash, prev_block->hash, HASH_SIZE - 1);
        block->prev_hash[HASH_SIZE - 1] = '\0'; // Ensure null-terminated
    }

    mineBlock(block, 4); // Default difficulty for mining
    blockchain->length++;
    return 1;
}

int isValidNewBlock(const Block *new_block, const Block *prev_block) {
    if (new_block->index != prev_block->index + 1 ||
        new_block->timestamp <= prev_block->timestamp ||
        strcmp(new_block->prev_hash, prev_block->hash) != 0) {
        return 0;
    }

    if (strcmp(calculateHash(new_block), new_block->hash) != 0) {
        return 0;
    }
    return 1;
}

int isValidChain(const Blockchain *blockchain) {
    if (blockchain->length == 0) {
        return 1; // Empty blockchain is valid
    }

    for (uint32_t i = 1; i < blockchain->length; i++) {
        if (!isValidNewBlock(&blockchain->chain[i], &blockchain->chain[i - 1])) {
            return 0;
        }
    }
    return 1;
}

void mineBlock(Block *block, uint32_t difficulty) {
    char prefix[difficulty + 1];
    memset(prefix, '0', difficulty);
    prefix[difficulty] = '\0';

    do {
        block->nonce++;
        strcpy(block->hash, calculateHash(block));
    } while (strncmp(block->hash, prefix, difficulty) != 0);
}

char *calculateHash(const Block *block) {
    char data[256]; // Placeholder for block data
    sprintf(data, "%d%lu%s%d", block->index, block->timestamp, block->prev_hash, block->nonce);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, strlen(data));
    SHA256_Final(hash, &sha256);

    static char hex_hash[HASH_SIZE];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&hex_hash[i * 2], "%02x", hash[i]);
    }
    hex_hash[HASH_SIZE - 1] = '\0'; // Ensure null-terminated
    return hex_hash;
}

void printBlock(const Block *block) {
    printf("Block %d:\n", block->index);
    printf("  Timestamp: %lu\n", block->timestamp);
    printf("  Previous Hash: %s\n", block->prev_hash);
    printf("  Nonce: %d\n", block->nonce);
    printf("  Hash: %s\n", block->hash);
}

void printBlockchain(const Blockchain *blockchain) {
    if (blockchain->length == 0) {
        printf("Blockchain is empty.\n");
        return;
    }

    printf("\nBlockchain:\n");
    for (uint32_t i = 0; i < blockchain->length; i++) {
        printBlock(&blockchain->chain[i]);
    }
}

void displayMenu() {
    printf("\nBlockchain Menu:\n");
    printf("1. Add a Block\n");
    printf("2. Print Blockchain\n");
    printf("3. Exit\n");
}

int main() {
    Blockchain blockchain;
    initBlockchain(&blockchain);
    int choice;

    do {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (addBlock(&blockchain)) {
                    printf("Block added successfully!\n");
                } else {
                    printf("Failed to add block. Blockchain capacity reached.\n");
                }
                break;
            case 2:
                printBlockchain(&blockchain);
                break;
            case 3:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please enter a valid option.\n");
        }
    } while (choice != 3);

    return 0;
}
