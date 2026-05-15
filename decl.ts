
export interface LayoutOptions4 {
    "PyPTO.filePath": string;
    "PyPTO.renderMode": string;
    "PyPTO.showTensor": string;
    "elk.direction": string;
    "org.eclipse.elk.algorithm": string;
    "org.eclipse.elk.layered.nodePlacement.strategy": string;
    "org.eclipse.elk.layered.spacing.nodeNodeBetweenLayers": string;
}

export interface TestInterface {
    "org.eclipse.elk.algorithm": string;
    "org.eclipse.elk.layered.nodePlacement.strategy": string;
    "org.eclipse.elk.layered.spacing.nodeNodeBetweenLayers": string;
}

export interface ElkLayout {
    children: TestInterface[];
    id: string;
    layoutOptions: LayoutOptions4;
}
