#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import numpy as np
import torch
import os

from flim.experiments import utils

def get_device():
    gpu = torch.cuda.is_available()

    if not gpu:
        device = torch.device('cpu')
    else:
        device = torch.device(0)

    return device

def load_args():
    parser = argparse.ArgumentParser(description='Create FLIM network and extract features.')
    parser.add_argument('-i', dest='input_path', type=str, required=True,
                        help='Path to folder containing the input images and markers')
    parser.add_argument('-a', dest='arch_path', type=str, required=True,
                        help='Path to the FLIM architecture (.json)')
    parser.add_argument('-o', dest='output_path', type=str, required=True,
                        help='Dir to write output activations')

    return parser.parse_args()

def extract_features(args):

    get_device()

    architecture = utils.load_architecture(args.arch_path)
    images, markers = utils.load_images_and_markers(args.input_path)
    # relabel_markers=True will set a new label for each connected component in the markers
    creator = LCNCreator(architecture, images=images, markers=markers, relabel_markers=False, device=device)
    # Build the feature extractor using FLIM
    creator.build_model()
    # model is a PyTorch Module https://pytorch.org/docs/stable/generated/torch.nn.Module.html
    model = creator.get_LIDSConvNet()

    # input mut be a PyTorch Tensor with shape (N, C, H, W)
    x = torch.from_numpy(images).permute(0, 3, 1, 2).float().to(device)
    features = model.forward(x)

    print(features.size())

    #mimg2save = act.squeeze(0).detach().cpu().numpy().transpose(3,2,1,0)
    #utils.save_mimage('out.mimg', mimg2save)


def main():

    args = load_args()
    print("rodou")
    os.mknod("newfile.txt")
    #extract_features(args)


if __name__ == "__main__":
    main()
