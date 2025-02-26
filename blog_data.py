from dataclasses import dataclass, field
from typing import List, Optional, Sequence
from datetime import date

@dataclass
class Post:
    tag: str
    url: str
    title: str
    lead: str
    date: date
    img: str
    read_time: int


class Vars:
    header_title = "Blog"

    posts = [
        Post(
            tag='termo',
            url="termo_linalg.html",
            title='Analysis of Ornstein-Uhlenbeck Process for Linear Systems',
            lead='''
            This blog post delves into the Ornstein-Uhlenbeck (OU) process and its application in the analysis of linear systems. We explore how the OU process, when applied to a Positive Semi-Definite (PSD) matrix and a specific vector, leads to more efficient and accurate sampling strategies.
            ''',
            date=date(2024, 1, 31),
            img='../feature_imgs/termo_linalg.png',
            read_time=1,
        ),
        Post(
            tag='ziplm',
            url="https://github.com/thomasahle/ziplm",
            title='ZipLm with Beam Search, BPE and Progressive Compression',
            lead='''
            ZipLM uses Gzip to create a language model, however it is quite bad. This post shows how to use beam search to improve the output of ZipLM. When sampling sentences from "The Great Gatsby," the raw model produces near-gibberish. However, implementing beam search yields increasingly coherent sentences as the beam width increases, although performance is still poor compared to older models.
            ''',
            date=date(2023, 8, 28),
            img='../feature_imgs/ziplm.png',
            read_time=1,
        ),
        Post(
            tag='einsum',
            url="einsum_to_dot.html",
            title='Convert Einsum to Tensor Network',
            lead='A technical guide on converting Einstein summation to tensor network operations.',
            date=date(2023, 1, 1),  # Replace with the correct date
            img='../feature_imgs/tensor.png',  # Replace with the actual image filename if available
            read_time=1,
        ),
        Post(
            tag='liars_dice',
            url="https://towardsdatascience.com/3bbed6addde0",
            title="Liar’s Dice by Self-Play",
            lead='I’ve been meaning to learn about AIs for games like Poker or Liar’s Dice for a while. Recently while reading Deepmind’s article Player of Games, I thought I might be able to make something really simple that works well enough to be fun. “It shouldn’t take more than an afternoon,” I thought. Of course, I ended up spending much more time on it, but it did turn out to be fun and very simple.',
            date=date(2022, 1, 1),  # Replace with the correct date
            img='../feature_imgs/dudo.png',
            read_time=1,
        ),
        Post(
            tag='sets',
            url="sets.html",
            title="An Evolutionary Data Structure for Sets",
            lead='I recently published a preprint, together with Jakob Knudsen, describing a new, optimal data structure for set similarity search. An interesting way to look at this is as an Evolutionary Algorithm (aka. Genetic Algorithm). We think it\'s probably the first time the provably best way to solve a problem has been evolutionary. This post won\'t go deep into the mathematics, but will contain code samples and some fun gadgets I built to simulate it.',
            date=date(2020, 1, 1),  # Replace with the correct date
            img='../feature_imgs/sets.png',  # Replace with the actual image filename if available
            read_time=1,
        ),
        Post(
            tag='hypergeometric',
            url="https://ahlenotes.wordpress.com/2015/12/08/hypergeometric_tail/",
            title="Another Tail of the Hypergeometric Distribution",
            lead='This blog post sheds light on the less-discussed sampling without replacement scenario, its implications in algorithm analysis, and how it compares with its more famous counterpart, the binomial distribution.',
            date=date(2015, 12, 8),
            img='../feature_imgs/hyper.png',
            read_time=1,
        ),
    ]

