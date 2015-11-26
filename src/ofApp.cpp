#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofDisableArbTex();
    
    // Thetaの設定(USB接続)
    devices = theta.listDevices();
    bool isDeviceConnected = false;
    for(int i = 0; i < devices.size(); i++){
        if(devices[i].deviceName == "RICOH THETA S"){
            theta.setDeviceID(devices[i].id);
            isDeviceConnected = true;
            maxSource = 3;
        }
    }
    if(!isDeviceConnected){
        ofLog(OF_LOG_ERROR, "RICOH THETA S is not connected.");
        maxSource = 2;
    }
    theta.initGrabber(1280, 720);
    
    // 環境マップ用の静止画
    envImage.load("wadabori_park.jpg");
    
    // 環境マップ用の動画
    ofDisableArbTex();
    videoPlayer.getTexturePtr()->allocate(1920, 960, GL_RGB);
    videoPlayer.load("ohmiya_shrine.mp4");
    videoPlayer.setLoopState(OF_LOOP_NORMAL);
    
    // 3Dモデル
    model.loadModel("dragon.obj");
    modelScale = model.getModelMatrix().getScale();
    
    // Dual fish-eyeからEquirectangularに変換するシェーダー
    equirectShader.load("shaders/equirectanguler");
    
    // 環境マップ用のシェーダー
    envMapShader.load("shaders/equirectEnvMap");
    
    // 背景用のPrimitive Sphere
    envSphere = ofSpherePrimitive(2000, 64).getMesh();
    for(int i=0;i<envSphere.getNumTexCoords();i++){
        envSphere.setTexCoord(i, ofVec2f(1.0) - envSphere.getTexCoord(i));
    }
    for(int i=0;i<envSphere.getNumNormals();i++){
        envSphere.setNormal(i, envSphere.getNormal(i) * ofVec3f(-1));
    }
    
    // 変換後の動画書き込みに使用するFBO
    fbo.allocate(1920, 1080);
    
    // 環境マップの素材
    source = 0;
    
    object = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // 環境マップ用の素材の更新
    switch (source) {
        case 0:
            break;
            
        case 1:
            videoPlayer.update();
            break;
            
        case 2:
            theta.update();
            break;
            
        default:
            break;
    }
    
    // 3Dモデルのアニメーションの更新
    switch (object) {
        case 0:
            break;
            
        case 1:
            model.update();
            break;
            
        case 2:
            break;
            
        default:
            break;
    }
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // テクスチャー作成
    ofDisableDepthTest();
    switch (source) {
        case 0:
            envTex = envImage.getTexture();
            break;
            
        case 1:
            if(videoPlayer.isFrameNew()){
                fbo.begin();
                ofClear(0);
                ofEnableArbTex();
                videoPlayer.draw(0, 0, 1920, 960);
                ofDisableArbTex();
                fbo.end();
                envTex = fbo.getTexture();
            }
            break;
            
        case 2:
            if(theta.isFrameNew()){
                fbo.begin();
                ofClear(0);
                equirectShader.begin();
                equirectShader.setUniformTexture("mainTex", theta.getTexture(), 0);
                equirectShader.setUniform1f("radius", 0.445);
                equirectShader.setUniform4f("uvOffset", 0, 0, 0, 0);
                theta.draw(0, 0, 1280, 640);
                equirectShader.end();
                fbo.end();
                envTex = fbo.getTexture();
            }
            break;
            
        default:
            break;
    }
    
    // 描画
    ofEnableDepthTest();
    cam.begin();
    
    envTex.bind();
    envSphere.draw();
    envTex.unbind();
    
    envMapShader.begin();
    envMapShader.setUniformTexture("envMap", envTex, 0);
    envMapShader.setUniformMatrix4f("viewTranspose", ofMatrix4x4::getTransposedOf(cam.getModelViewMatrix()));
    
    switch (object) {
        case 0:
        case 1:
            ofPushMatrix();
            ofTranslate(0, -150);
            for(int i=0; i<model.getNumMeshes(); i++){
                ofPushMatrix();
                ofMultMatrix(model.getMeshHelper(i).matrix);
                ofScale(modelScale.x, modelScale.y, modelScale.z);
                model.getCurrentAnimatedMesh(i).draw();
                ofPopMatrix();
            }
            ofPopMatrix();
            break;
        
        case 2:
            ofDrawSphere(0, 0, 300);
            break;
    }
    
    envMapShader.end();
    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key) {
        case OF_KEY_RIGHT:
            source ++;
            source %= maxSource;
            switch (source) {
                case 0:
                    videoPlayer.stop();
                    break;
                    
                case 1:
                    fbo.allocate(1920, 960);
                    videoPlayer.play();
                    break;
                    
                case 2:
                    fbo.allocate(1280, 640);
                    videoPlayer.stop();
                    break;
                    
                default:
                    break;
            }
            break;
            
        case OF_KEY_DOWN:
            object ++;
            object %= 3;
            switch (object) {
                case 0:
                    model.loadModel("dragon.obj");
                    modelScale = model.getModelMatrix().getScale();
                    break;
                    
                case 1:
                    model.loadModel("astroBoy_walk.dae", true);
                    model.setLoopStateForAllAnimations(OF_LOOP_NORMAL);
                    model.playAllAnimations();
                    modelScale = model.getModelMatrix().getScale();
                    break;
                    
                case 2:
                    break;
                    
                default:
                    break;
            }
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
