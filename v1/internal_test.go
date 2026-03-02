package tcobsv1

import (
	"bytes"
	"errors"
	"io"
	"strings"
	"testing"
)

func TestSigilAndOffset(t *testing.T) {
	tests := []struct {
		name       string
		in         byte
		wantSigil  int
		wantOffset int
	}{
		{name: "NOP", in: N | 31, wantSigil: N, wantOffset: 31},
		{name: "Z2", in: Z2 | 5, wantSigil: Z2, wantOffset: 5},
		{name: "R4", in: R4 | 7, wantSigil: R4, wantOffset: 7},
		{name: "Reserved", in: 0x03, wantSigil: Reserved, wantOffset: 3},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			gotSigil, gotOffset := sigilAndOffset(tt.in)
			if gotSigil != tt.wantSigil || gotOffset != tt.wantOffset {
				t.Fatalf("sigilAndOffset(%#x) = (%#x,%d), want (%#x,%d)",
					tt.in, gotSigil, gotOffset, tt.wantSigil, tt.wantOffset)
			}
		})
	}
}

func TestRepeatByte(t *testing.T) {
	b, err := repeatByte(0, []byte{0xAA, N})
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if b != 0xAA {
		t.Fatalf("got %#x, want %#x", b, 0xAA)
	}

	b, err = repeatByte(3, []byte{0xBB})
	if err != nil {
		t.Fatalf("unexpected error: %v", err)
	}
	if b != 0xBB {
		t.Fatalf("got %#x, want %#x", b, 0xBB)
	}
}

func TestRepeatByteErrors(t *testing.T) {
	if _, err := repeatByte(0, nil); err == nil {
		t.Fatal("expected error for offset 0 and empty input")
	}
	if _, err := repeatByte(1, nil); err == nil {
		t.Fatal("expected error for offset > 0 and empty input")
	}
}

func TestDecodeErrorPaths(t *testing.T) {
	d := make([]byte, 8)

	_, err := Decode(d, []byte{Reserved})
	if err == nil || !strings.Contains(err.Error(), "reserved sigil") {
		t.Fatalf("expected reserved sigil error, got %v", err)
	}

	_, err = Decode(d, []byte{N | 1})
	if err == nil || !strings.Contains(err.Error(), "sigil chain mismatch") {
		t.Fatalf("expected sigil chain mismatch, got %v", err)
	}

	_, err = Decode(make([]byte, 1), []byte{0xAA, 0xAA, N | 2})
	if err == nil || !strings.Contains(err.Error(), "data buffer too small") {
		t.Fatalf("expected data buffer too small, got %v", err)
	}

	_, err = Decode(d, []byte{R2})
	if err == nil || !strings.Contains(err.Error(), "inconsistent TCOBS data") {
		t.Fatalf("expected inconsistent data error, got %v", err)
	}
}

type shortWriter struct {
	limit int
	buf   bytes.Buffer
}

func (w *shortWriter) Write(p []byte) (int, error) {
	if w.limit < len(p) {
		if w.limit > 0 {
			_, _ = w.buf.Write(p[:w.limit])
		}
		return w.limit, nil
	}
	return w.buf.Write(p)
}

func TestEncoderShortWriteAndFlush(t *testing.T) {
	w := &shortWriter{limit: 2}
	enc := NewEncoder(w, 32)

	n, err := enc.Write([]byte{0xAA, 0xAA, 0xAA})
	if n != 3 {
		t.Fatalf("Write() n=%d, want 3", n)
	}
	if err == nil {
		t.Fatal("expected short-write error")
	}

	for i := 0; i < 4 && enc.iCnt > 0; i++ {
		_ = enc.Flush()
	}
	if enc.iCnt != 0 {
		t.Fatalf("expected flushed pending data, still have %d bytes", enc.iCnt)
	}
	if !bytes.Equal(w.buf.Bytes(), []byte{0xAA, 0x09, 0x00}) {
		t.Fatalf("unexpected output %v", w.buf.Bytes())
	}
}

func TestEncoderWriteRequiresFlushWhenPending(t *testing.T) {
	w := &shortWriter{limit: 0}
	enc := NewEncoder(w, 32)

	if _, err := enc.Write([]byte{0xAA}); err == nil {
		t.Fatal("expected first write to require Flush after short write")
	}
	if _, err := enc.Write([]byte{0xBB}); err == nil || !strings.Contains(err.Error(), "needs Flush") {
		t.Fatalf("expected needs Flush error, got %v", err)
	}
}

type fixedErrReader struct {
	err error
}

func (r fixedErrReader) Read(_ []byte) (int, error) {
	return 0, r.err
}

func TestDecoderReadPropagatesReaderError(t *testing.T) {
	wantErr := io.ErrUnexpectedEOF
	dec := NewDecoder(fixedErrReader{err: wantErr}, 16)

	n, err := dec.Read(make([]byte, 16))
	if n != 0 {
		t.Fatalf("Read() n=%d, want 0", n)
	}
	if !errors.Is(err, wantErr) {
		t.Fatalf("Read() err=%v, want %v", err, wantErr)
	}
}
