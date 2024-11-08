#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "htslib/sam.h"

#include <stdio.h>
#include <stdlib.h>
#include <htslib/sam.h>

int main(int argc, char **argv) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s <input.bam> <output_odd.bam> <output_even.bam> <divide number>\n", argv[0]);
        return 1;
    }
    int divide = atoi(argv[4]);
    samFile *in = sam_open(argv[1], "r");
    if (in == NULL) {
        fprintf(stderr, "Error opening input BAM file.\n");
        return 1;
    }

    samFile *out_odd = sam_open(argv[2], "wb");
    if (out_odd == NULL) {
        fprintf(stderr, "Error opening output odd BAM file.\n");
        sam_close(in);
        return 1;
    }

    samFile *out_even = sam_open(argv[3], "wb");
    if (out_even == NULL) {
        fprintf(stderr, "Error opening output even BAM file.\n");
        sam_close(in);
        sam_close(out_odd);
        return 1;
    }

    bam_hdr_t *header = sam_hdr_read(in);
    if (header == NULL) {
        fprintf(stderr, "Error reading BAM header.\n");
        sam_close(in);
        sam_close(out_odd);
        sam_close(out_even);
        return 1;
    }

    if (sam_hdr_write(out_odd, header) < 0 || sam_hdr_write(out_even, header) < 0) {
        fprintf(stderr, "Error writing BAM header.\n");
        bam_hdr_destroy(header);
        sam_close(in);
        sam_close(out_odd);
        sam_close(out_even);
        return 1;
    }
    bam1_t *aln = bam_init1();
    int read_count = 0;

    while (sam_read1(in, header, aln) >= 0) {
        if (read_count % divide == 0) {
            if (sam_write1(out_even, header, aln) < 0) {
                fprintf(stderr, "Error writing to target BAM file.\n");
                break;
            }
        } else {
            if (sam_write1(out_odd, header, aln) < 0) {
                fprintf(stderr, "Error writing to rest BAM file.\n");
                break;
            }
        }
        read_count++;
    }

    bam_destroy1(aln);
    bam_hdr_destroy(header);
    sam_close(in);
    sam_close(out_odd);
    sam_close(out_even);

    return 0;
}