"use client";
import "leaflet/dist/leaflet.css";
import "../styles/Home.css";

import { Box } from "@radix-ui/themes";

import { MapContainer } from "react-leaflet/MapContainer";
import { Marker } from "react-leaflet/Marker";
import { Popup } from "react-leaflet/Popup";
import { TileLayer } from "react-leaflet/TileLayer";
import { LayersControl } from "react-leaflet";
import type { IconOptions } from "leaflet";
import L from "leaflet";

// import { Polyline} from "react-leaflet";

function CarMap() {
	const carIconOptions: IconOptions = {
		iconUrl: "car-marker.png",
		iconSize: [ 64.5,38.7], //645, 387
		iconAnchor: [22, 94],
		popupAnchor: [-3, -76],
	};
	const carIcon = L.icon(carIconOptions);
	return (
		<MapContainer
			className="map"
			center={[9.53405, 6.45567]}
			zoom={17}
			scrollWheelZoom={false}
			zoomAnimation={true}
		>
			<TileLayer
				attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
				url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
			/>
			{/* <Marker position={[51.505, -0.09]}>
    				<Popup>
    					A pretty CSS3 popup. <br /> Easily customizable.
    				</Popup>
    			</Marker> */}
			{/* <Polyline positions={[]}/> */}
			<LayersControl>
				<LayersControl.Overlay checked name="Car Layer">
					<Marker position={[9.53405, 6.45567]} icon={carIcon}>
						<Popup>Car Marker</Popup>
					</Marker>
				</LayersControl.Overlay>
			</LayersControl>
		</MapContainer>
	);
}

export default CarMap;
