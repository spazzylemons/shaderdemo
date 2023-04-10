#version 330 core

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec4 colDiffuse;

uniform vec3 crackColor;
uniform vec3 surfaceColor;

out vec4 finalColor;

// selected based on ~~random dice roll~~ python script, no special attributes
const vec3[] RANDOM_POINTS = vec3[](
    vec3(0.48444445887044096, 0.4963186140354179, 0.43388680070654884),
    vec3(0.45028291676442467, 0.6118947787439907, 0.10546104031413495),
    vec3(0.38401992805875695, 0.48979474790669575, 0.9169666664135929),
    vec3(0.5520195416237719, 0.6307946920784716, 0.36100029711318415),
    vec3(0.5808864360564767, 0.15785863605010186, 0.5084955185212752),
    vec3(0.1351340292504698, 0.26559431861474914, 0.7244956852321002),
    vec3(0.015791655071845878, 0.12276405036065685, 0.3197061513514359),
    vec3(0.9686947607409647, 0.6578216705940506, 0.454681565975102),
    vec3(0.006886828876172646, 0.4852672330344654, 0.22352252362388925),
    vec3(0.47131019900494076, 0.5442919986729376, 0.4242131300297316),
    vec3(0.7143398987403957, 0.2332318351904984, 0.9965167270778196),
    vec3(0.8963577808198675, 0.6292919692048702, 0.9486588176712635),
    vec3(0.766063781600339, 0.39731706797109356, 0.32143729855135283),
    vec3(0.28455747038074697, 0.6120751961542646, 0.2768479838634682),
    vec3(0.7903073298309652, 0.5963671029055481, 0.5386084668014715),
    vec3(0.3096001882771974, 0.4248459156548411, 0.8752868071695085),
    vec3(0.54791307933329, 0.025426186767923853, 0.5586059195209855),
    vec3(0.0734462719746809, 0.8068492016739139, 0.6559549400947845),
    vec3(0.4130801067631479, 0.49014334840981033, 0.928937492955749),
    vec3(0.4853968197828076, 0.28535679638095335, 0.35994756801500083)
);

const int NUM_RANDOM_POINTS = 20;

const vec3[] OFFSETS = vec3[](
    vec3(1.0, -0.3535533905932738, 0.0),
    vec3(-0.5, -0.3535533905932738, 0.8660254037844386),
    vec3(-0.5, -0.3535533905932738, -0.8660254037844386),
    vec3(0.0, 1.0606601717798214, 0.0)
);

const float OFFSET_SCALE = 0.01;

int checkIndex(vec3 point) {
    int closestIndex = 0;
    // literal resolves to infinity
    float closestDistance = 1e309;
    // scale by half to spread out points more, and mod to loop point space
    point = mod(point * 0.5, 1.0);
    for (int i = 0; i < NUM_RANDOM_POINTS; i++) {
        // check point in random point cloud
        vec3 cloudPoint = RANDOM_POINTS[i];
        // find closest distance vector in repeating space
        vec3 distInfo = round(point - cloudPoint);
        vec3 distVec = point - (cloudPoint + distInfo);
        // distance squared check
        float d = dot(distVec, distVec);
        if (d < closestDistance) {
            closestDistance = d;
            closestIndex = i;
        }
    }
    return closestIndex;
}

void main() {
    // check four offsets that are equidistant from each other and from the
    // fragment location. if they do not all have the same cell index, then
    // we draw an edge instead.
    int lastIndex = checkIndex(fragPosition + OFFSETS[0] * OFFSET_SCALE);
    for (int i = 1; i < 4; i++) {
        int newIndex = checkIndex(fragPosition + OFFSETS[i] * OFFSET_SCALE);
        if (newIndex != lastIndex) {
            lastIndex = -1;
            break;
        }
    }
    if (lastIndex == -1) {
        // black edges in between cells
        finalColor = vec4(crackColor, 1.0);
    } else {
        // toon-ish shading
        float toonShade = dot(fragNormal, vec3(0.5773502691896257, 0.5773502691896257, 0.5773502691896257));
        toonShade = 8.0 * toonShade - 4.0;
        toonShade = clamp(((sign(toonShade) * pow(toonShade, 0.8)) + 3.0) * 0.25, 0.5, 1.0);
        finalColor = vec4(toonShade * surfaceColor, 1.0);
    }
    // apply tint
    finalColor *= colDiffuse * fragColor;
}
