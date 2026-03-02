package tcobsv2

import (
	"bytes"
	"errors"
	"io"
	"strings"
	"testing"
)

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

func TestCEncodeAndCDecodeEmptyInput(t *testing.T) {
	if n := CEncode(make([]byte, 8), nil); n != 0 {
		t.Fatalf("CEncode empty input n=%d, want 0", n)
	}
	if n := CDecode(make([]byte, 8), nil); n != 0 {
		t.Fatalf("CDecode empty input n=%d, want 0", n)
	}
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
	if !bytes.Equal(w.buf.Bytes(), []byte{0xAA, 0x81, 0x00}) {
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
