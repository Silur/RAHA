use criterion::{black_box, criterion_group, criterion_main, Criterion};
use merkle::MerkleTree;
use ring::digest::{Algorithm, SHA512};
static DIGEST: &Algorithm = &SHA512;

fn gen_bench(c: &mut Criterion) {
    let mut group = c.benchmark_group("size 10");
    group.significance_level(0.1).sample_size(10);
    let values = vec![vec![1u8; 1024]; 65536];
    group.bench_function("Merkle SHA512 gen", |b| {
        b.iter(|| MerkleTree::from_vec(DIGEST, black_box(values.clone())))
    });
    group.finish();
}

fn proof_bench(c: &mut Criterion) {
    let mut group = c.benchmark_group("size 10");
    group.significance_level(0.1).sample_size(10);
    let values = vec![vec![1u8; 1024]; 65536];
    let tree = MerkleTree::from_vec(DIGEST, values.clone());
    group.bench_function("Merkle SHA512 proof", |b| {
        b.iter(|| {
            tree.gen_proof(black_box(values[0].clone()));
        })
    });
    group.finish();
}

fn check_bench(c: &mut Criterion) {
    let mut group = c.benchmark_group("size 10");
    group.significance_level(0.1).sample_size(10);
    let values = vec![vec![1u8; 1024]; 65536];
    let tree = MerkleTree::from_vec(DIGEST, values.clone());
    let proof = tree.gen_proof(black_box(values[0].clone())).unwrap();
    group.bench_function("Merkle SHA512 check", |b| {
        b.iter(|| {
                proof.validate(black_box(tree.root_hash()));
        })
    });
    group.finish();
}

criterion_group!(benches, gen_bench, proof_bench, check_bench);
criterion_main!(benches);
