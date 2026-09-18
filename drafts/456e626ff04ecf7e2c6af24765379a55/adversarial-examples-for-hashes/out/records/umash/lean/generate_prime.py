from pathlib import Path
source = Path('MakePrimes.py').read_text()
source = source.replace('ProvenHashes.Classic', 'ProvenHashes.UMASH')
source = source.replace('2**130-5', '2**61-1').replace('2 ^ 130 - 5', '2 ^ 61 - 1')
source = source.replace('poly1305_prime', 'mersenne61_prime')
source = source.replace("Path('ProvenHashes/ClassicPrimes.lean')", "Path('ProvenHashes/UMASHPrime.lean')")
exec(source)
p = Path('ProvenHashes/UMASHPrime.lean')
text = p.read_text().replace('end ProvenHashes.UMASH',
    'def p : ℕ := 2 ^ 61 - 1\ninstance : Fact p.Prime := ⟨mersenne61_prime⟩\nend ProvenHashes.UMASH')
p.write_text(text)
