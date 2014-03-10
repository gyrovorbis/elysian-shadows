//
//  Shader.fsh
//  libGyroiOSBasic
//
//  Created by Falco Girgis on 9/11/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
