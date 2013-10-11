
<!DOCTYPE html>
<html lang="en">
    <head>
        <title> o3dgc decoding</title>
        <meta charset="utf-8">
        <meta name="viewport" content="width=device-width, user-scalable=no, minimum-scale=1.0, maximum-scale=1.0">
        <style>
            body {
                color: #cccccc;
                font-family:Monospace;
                font-size:13px;
                text-align:center;
                background-color: #050505;
                margin: 0px;
                overflow: hidden;
            }
            #info {
                position: absolute;
                top: 0px; width: 100%;
                padding: 5px;
            }
            a {
                color: #0080ff;
            }
        </style>
        <script>
        var container;
        var camera, scene, renderer;
        var geometry, mesh, ifs, material;
        var oReq = new XMLHttpRequest();
        var D = 100;
        var compressedStream;
        var resType = "arraybuffer";
<?php
    if (isset($_GET["obj"]) && $_GET["obj"] == 'true'){
        echo "        var dump = true;\n";
    }
    else{
        echo "        var dump = false;\n";
    }
    if (isset($_GET["model"]) && ($_GET["model"] == 'lift.s3d' || $_GET["model"] == 'lift_ascii.s3d')){
        echo "        dump = true;\n";
        echo "        var fileName = '".$_GET["model"]."';\n";
        echo "        var dynamicVectorDecoding = true;\n";
    }
    else if (isset($_GET["model"]) && $_GET["model"] != ''){
        echo "        var fileName = '".$_GET["model"]."';\n";
        echo "        var dynamicVectorDecoding = false;\n";
    }
    else
    {
        echo "        var fileName = 'duck.s3d';\n";
        echo "        var dynamicVectorDecoding = false;\n";
    }
?>
        if (fileName.indexOf("ascii") !== -1){
            resType = "text";
        }
        oReq.open("GET", fileName, true);
        oReq.responseType = resType;
        oReq.onload = function (oEvent) {
            compressedStream = oReq.response;
        };
        oReq.send(null);
        </script>
        <script type="text/javascript" src="o3dgc.js"></script>
        <script>
		function print(txt){
			var text2 = document.createElement('div');
			text2.style.position = 'absolute';
			text2.style.width = 100;
			text2.style.height = 100;
			text2.style.backgroundColor = 'transparent';
			text2.innerHTML = txt;
			text2.style.top =  100 + 'px';
			text2.style.left =  0 + 'px';
			text2.style.textAlign='left';
			document.body.appendChild(text2);
		}
        function decodeDV(bstream, size){
            var decoder = new o3dgc.DynamicVectorDecoder();
            var dynamicVector = new o3dgc.DynamicVector();
            var timer = new o3dgc.Timer();
            timer.Tic();
            decoder.DecodeHeader(dynamicVector, bstream);
            timer.Toc();
            console.log("DecodeHeader time (ms) " + timer.GetElapsedTime());
            // allocate memory
            if (dynamicVector.GetNVector() > 0 && dynamicVector.GetDimVector()) {
                dynamicVector.SetVectors(new Float32Array(dynamicVector.GetNVector() * dynamicVector.GetDimVector()));
                dynamicVector.SetMinArray(new Float32Array(dynamicVector.GetDimVector()));
                dynamicVector.SetMaxArray(new Float32Array(dynamicVector.GetDimVector()));
                dynamicVector.SetStride(dynamicVector.GetDimVector());
            }
            console.log("Dynamic vector info ");
            console.log("\t# vectors   " + dynamicVector.GetNVector());
            console.log("\tdim         " + dynamicVector.GetDimVector());
            // decode DV
            timer.Tic();
            decoder.DecodePlayload(dynamicVector, bstream);
            timer.Toc();
            console.log("DecodePlayload time " + timer.GetElapsedTime() + " ms, " + size + " bytes (" + (8.0 * size / dynamicVector.GetNVector()) + " bpv)");
            SaveDV(dynamicVector, fileName);
        }
        function decodeIFS(bstream, size){
            var decoder = new o3dgc.SC3DMCDecoder();
            var timer = new o3dgc.Timer();
            ifs = new o3dgc.IndexedFaceSet();
            timer.Tic();
            decoder.DecodeHeader(ifs, bstream);
            timer.Toc();
			var headerDecodeTime = timer.GetElapsedTime();
			var headerSize = decoder.m_iterator.m_count;
            // allocate memory
            if (ifs.GetNCoordIndex() > 0) {
                ifs.SetCoordIndex(new Uint16Array(3 * ifs.GetNCoordIndex()));
            }
            if (ifs.GetNCoord() > 0) {
                ifs.SetCoord(new Float32Array(3 * ifs.GetNCoord()));
            }
            if (ifs.GetNNormal() > 0) {
                ifs.SetNormal(new Float32Array(3 * ifs.GetNNormal()));
            }
            var numNumFloatAttributes = ifs.GetNumFloatAttributes();
            for (var a = 0; a < numNumFloatAttributes; ++a){
                if (ifs.GetNFloatAttribute(a) > 0){
                    ifs.SetFloatAttribute(a, new Float32Array(ifs.GetFloatAttributeDim(a) * ifs.GetNFloatAttribute(a)));
                }
            }
            var numNumIntAttributes = ifs.GetNumIntAttributes();
            for (var a = 0; a < numNumIntAttributes; ++a){
                if (ifs.GetNIntAttribute(a) > 0){
                    ifs.SetIntAttribute(a, new Int32Array(ifs.GetIntAttributeDim(a) * ifs.GetNIntAttribute(a)));
                }
            }
            // decode mesh
            timer.Tic();
            decoder.DecodePlayload(ifs, bstream);
            timer.Toc();
			var stats = decoder.GetStats();			var log = '<pre>';
            log += "Mesh info \n";
			log += "  # coords    " + ifs.GetNCoord() + "\n";
			log += "  # normals   " + ifs.GetNNormal() + "\n";
            for (var a = 0; a < numNumFloatAttributes; ++a){
				log += "  # FloatAttribute[" + a + "] " + ifs.GetNFloatAttribute(a) + "(" + ifs.GetFloatAttributeType(a)+")\n";
            }
            for (var a = 0; a < numNumIntAttributes; ++a){
				log += "  # IntAttribute[" + a + "] " + ifs.GetNIntAttribute(a) + "(" + ifs.GetIntAttributeType(a)+")" + "\n";
            }
			log += "  # triangles " + ifs.GetNCoordIndex() + "\n\n";

			log += "DecodeHeader\n";
			log += "  time       " + (headerDecodeTime).toFixed(0) + " ms\n";
			log += "  size       " + (headerSize/1024).toFixed(1) + " KB (" + (8.0 * headerSize / ifs.GetNCoord()).toFixed(1) + " bpv)" + "\n\n";
			log += "DecodePlayload\n";
			log += "  time       " + (timer.GetElapsedTime()).toFixed(0) + " ms\n";
			log += "  size       " + (size/1024).toFixed(1) + " KB (" + (8.0 * size / ifs.GetNCoord()).toFixed(1) + " bpv)" + "\n\n";
			log += "Details\n";
			log += "  CoordIndex " + (stats.m_timeCoordIndex).toFixed(0) + " ms, " + Math.round(stats.m_streamSizeCoordIndex/1024) + " KB (" + (8.0 * stats.m_streamSizeCoordIndex / ifs.GetNCoord()).toFixed(1) + " bpv)\n";
			log += "  Coord      " + (stats.m_timeCoord).toFixed(0) + " ms, " + Math.round(stats.m_streamSizeCoord/1024) + " KB (" + (8.0 * stats.m_streamSizeCoord / ifs.GetNCoord()).toFixed(1) + " bpv)\n";
			log += "  Normal     " + (stats.m_timeNormal).toFixed(0) + " ms, " + Math.round(stats.m_streamSizeNormal/1024) + " KB (" + (8.0 * stats.m_streamSizeNormal / ifs.GetNCoord()).toFixed(1) + " bpv)\n";
            for (var a = 0; a < numNumFloatAttributes; ++a){
				log += "  Float Attributes   " + (stats.m_timeFloatAttribute[a]).toFixed(0) + " ms, " + Math.round(stats.m_streamSizeFloatAttribute[a]/1024) + " KB (" + (8.0 * stats.m_streamSizeFloatAttribute[a] / ifs.GetNCoord()).toFixed(1) + " bpv)\n";
            }
            for (var a = 0; a < numNumIntAttributes; ++a){
				log += "  Int Attributes   " + (stats.m_timeIntAttribute[a]).toFixed(0) + " ms, " + Math.round(stats.m_streamSizeIntAttribute[a]/1024) + " KB (" + (8.0 * stats.m_streamSizeIntAttribute[a] / ifs.GetNCoord()).toFixed(1) + " bpv)\n";
            }
			log += "  Reorder    " + (stats.m_timeReorder).toFixed(1) + " ms,  " + 0 + " KB (" + 0.0 + " bpv)\n";
			log += '</pre>';
			print(log);
            if (dump){
                SaveOBJ(ifs, fileName);
            }
        }
        function decode(arrayBuffer) {
            if (arrayBuffer) {
                function str2ab(str) {
                    var buf = new ArrayBuffer(str.length);
                    var bufView = new Uint8Array(buf);
                    for (var i=0, strLen=str.length; i<strLen; i++) {
                        bufView[i] = str.charCodeAt(i);
                    }
                    return buf;
                }
                if (resType === "text") {
                    var bstream = new o3dgc.BinaryStream(str2ab(arrayBuffer));
                    var size = arrayBuffer.length;
                }
                else{
                    var bstream = new o3dgc.BinaryStream(arrayBuffer);
                    var size = arrayBuffer.byteLength;
                }
                if (dynamicVectorDecoding) {
                    decodeDV(bstream, size);
                }
                else {
                    decodeIFS(bstream, size);
                }
            }
        }
        </script>
        <script src="three.min.js"></script>
    </head>
    <body>

        <div id="container"></div>
        <div id="info"><a href="https://github.com/amd/rest3d/tree/master/server/o3dgc" target="_blank">o3dgc</a> compressed stream decoding </div>
        <script>
        if (dump){
            while (typeof compressedStream === "undefined" && !compressedStream) {
            }
            decode(compressedStream);
        }
        else {
            init();
            animate();
        }
        function init() {
            container = document.getElementById('container');
            camera = new THREE.PerspectiveCamera(45, window.innerWidth / window.innerHeight, 0.1, 1000.0);
            camera.position.z = 300;
            scene = new THREE.Scene();
            scene.add(new THREE.AmbientLight(0x444444));
            var light1 = new THREE.DirectionalLight(0xffffff, 0.5);
            light1.position.set(1, 1, 1);
            scene.add(light1);

            var light2 = new THREE.DirectionalLight(0xffffff, 1.5);
            light2.position.set(0, -1, 0);
            scene.add(light2);

            material = new THREE.MeshPhongMaterial({
                color: 0xaabbcc, ambient: 0xaaaaaa, specular: 0xffffff, shininess: 250,
                side: THREE.DoubleSide, vertexColors: THREE.VertexColors
            });
            renderer = new THREE.WebGLRenderer({ antialias: false, alpha: false });
            renderer.setClearColor(0x000000, 1);
            renderer.setSize(window.innerWidth, window.innerHeight);
            renderer.gammaInput = true;
            renderer.gammaOutput = true;
            renderer.physicallyBasedShading = true;
            container.appendChild(renderer.domElement);
            window.addEventListener('resize', onWindowResize, false);
        }
        function onWindowResize() {
            camera.aspect = window.innerWidth / window.innerHeight;
            camera.updateProjectionMatrix();
            renderer.setSize(window.innerWidth, window.innerHeight);
        }
        function animate() {
            requestAnimationFrame(animate);
            render();
        }
        function render() {
            if (typeof mesh !== "undefined") {
                var time = Date.now() * 0.001;
                mesh.rotation.x = time * 0.25;
                mesh.rotation.y = time * 0.5;
            }
            else if (typeof compressedStream !== "undefined" && compressedStream) {
                decode(compressedStream);
                var n = 3 * ifs.GetNCoord();
                geometry = new THREE.BufferGeometry();
                geometry.attributes = [];
                if (ifs.GetNCoordIndex() > 0) {
                    geometry.attributes.index = { itemSize: 1, array: ifs.GetCoordIndex(), numItems: ifs.GetNCoordIndex() * 3 };
                    geometry.offsets = [{ start: 0, index: 0, count: ifs.GetNCoordIndex() * 3}];
                }
                if (ifs.GetNCoord() > 0) {
                    geometry.attributes.position = { itemSize: 3, array: ifs.GetCoord(), numItems: ifs.GetNCoord() * 3 };
                }
                if (ifs.GetNNormal() > 0) {
                    geometry.attributes.normal = { itemSize: 3, array: ifs.GetNormal(), numItems: ifs.GetNNormal() * 3 };
                }
                var colors = new Float32Array(n);
                for(var i = 0; i < n; i+=3){
                    colors[i] = 1.0;
                    colors[i+1] = 1.0;
                    colors[i+2] = 0.0;
                }
                geometry.attributes.color = { itemSize: 3, array: colors, numItems: n };
                geometry.computeBoundingSphere();
                // center and scale
                var c = geometry.boundingSphere.center;
                var r = geometry.boundingSphere.radius;
                for (var i = 0; i < n; i += 3) {
                    geometry.attributes.position.array[i] = D * (geometry.attributes.position.array[i] - c.x) / r;
                    geometry.attributes.position.array[i + 1] = D * (geometry.attributes.position.array[i + 1] - c.y) / r;
                    geometry.attributes.position.array[i + 2] = D * (geometry.attributes.position.array[i + 2] - c.z) / r;
                }
                mesh = new THREE.Mesh(geometry, material);

                scene.add(mesh);
                meshAddedToScene = true;
            }
            renderer.render(scene, camera);
        }
        function SaveDV(dynamicVector, fileName) {
            var num = dynamicVector.GetNVector();
            var dim = dynamicVector.GetDimVector();
            document.writeln("#### <br>");
            document.writeln("# <br>");
            document.writeln("# DV File Generated by test_o3dgc<br>");
            document.writeln("#<br>");
            document.writeln("####<br>");
            document.writeln("# DV " + fileName + "<br>");
            document.writeln("#<br>");
            document.writeln("# Vectors:     " + num + "<br>");
            document.writeln("# Dim:         " + dim + "<br>");
            document.writeln("#<br>");
            document.writeln("####<br>");
            var vectors = dynamicVector.GetVectors();
            for (var i = 0; i < num; ++i) {
                document.writeln(i + ": ");
                for (var d = 0; d < dim; ++d) {
                    document.writeln(vectors[dim * i + d] + " ");
                }
                document.writeln("<br>");
            }
        }
        function SaveOBJ(ifs, fileName) {
            var triangles = ifs.GetCoordIndex();
            var points = ifs.GetCoord();
            var normals = ifs.GetNormal();
            var np = ifs.GetNCoord();
            var nn = ifs.GetNNormal();
            var nt = 0;
            var numNumFloatAttributes = ifs.GetNumFloatAttributes();
            for (var a = 0; a < numNumFloatAttributes; ++a){
                if (ifs.GetFloatAttributeType(a) === o3dgc.O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_TEXCOORD){
                    texCoords = ifs.GetFloatAttribute(a);
                    nt = ifs.GetNFloatAttribute(a);
                }
            }
            var nf = ifs.GetNCoordIndex();
            document.writeln("#### <br>");
            document.writeln("# <br>");
            document.writeln("# OBJ File Generated by test_o3dgc<br>");
            document.writeln("#<br>");
            document.writeln("####<br>");
            document.writeln("# Object " + fileName + "<br>");
            document.writeln("#<br>");
            document.writeln("# Coord:     " + np + "<br>");
            document.writeln("# Normals:   " + nn + "<br>");
            document.writeln("# TexCoord:  " + nt + "<br>");
            document.writeln("# Triangles: " + nf + "<br>");
            document.writeln("#<br>");
            document.writeln("####<br>");
            for (var i = 0; i < np; ++i) {
                document.writeln("v " + points[3 * i] + " " + points[3 * i + 1] + " " + points[3 * i + 2] + "<br>");
            }
            for (var i = 0; i < nn; ++i) {
                document.writeln("vn " + normals[3 * i] + " " + normals[3 * i + 1] + " " + normals[3 * i + 2] + "<br>");
            }
            for (var i = 0; i < nt; ++i) {
                document.writeln("vt " + texCoords[2 * i] + " " + texCoords[2 * i + 1] + "<br>");
            }
            if (nt > 0 && nn > 0) {
                for (var i = 0; i < nf; ++i) {
                    document.writeln("f " + (triangles[3 * i] + 1) + "/" + (triangles[3 * i] + 1) + "/" + (triangles[3 * i] + 1) + " " + (triangles[3 * i + 1] + 1) + "/" + (triangles[3 * i + 1] + 1) + "/" + (triangles[3 * i + 1] + 1) + " " + (triangles[3 * i + 2] + 1) + "/" + (triangles[3 * i + 2] + 1) + "/" + (triangles[3 * i + 2] + 1) + "<br>");
                }
            }
            else if (nt == 0 && nn > 0) {
                for (var i = 0; i < nf; ++i) {
                    document.writeln("f " + (triangles[3 * i] + 1) + "//" + (triangles[3 * i] + 1) + " " + (triangles[3 * i + 1] + 1) + "//" + (triangles[3 * i + 1] + 1) + " " + (triangles[3 * i + 2] + 1) + "//" + (triangles[3 * i + 2] + 1) + "<br>");
                }
            }
            else if (nt > 0 && nn == 0) {
                for (var i = 0; i < nf; ++i) {
                    document.writeln("f " + (triangles[3 * i] + 1) + "/" + (triangles[3 * i] + 1) + " " + triangles[3 * i + 1] + 1 + "/" + (triangles[3 * i + 1] + 1) + " " + (triangles[3 * i + 2] + 1) + "/" + (triangles[3 * i + 2] + 1) + "<br>");
                }
            }
            else {
                for (var i = 0; i < nf; ++i) {
                    document.writeln("f " + (triangles[3 * i] + 1) + " " + (triangles[3 * i + 1] + 1) + " " + (triangles[3 * i + 2] + 1) + "<br>");
                }
            }
        }
        </script>
    </body>
</html>
