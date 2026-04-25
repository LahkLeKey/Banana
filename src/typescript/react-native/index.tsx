// React Native entry (spec 011). Consumes only cross-platform exports from
// @banana/ui. Web-only Tailwind components are forbidden in this tree.
import { registerRootComponent } from 'expo';
import { StatusBar } from 'expo-status-bar';
import { SafeAreaView, Text, View } from 'react-native';
import { BananaBadge, RipenessLabel } from '@banana/ui/native';

const API_BASE_URL = process.env.EXPO_PUBLIC_BANANA_API_BASE_URL ?? '';

function App() {
    return (
        <SafeAreaView style={{ flex: 1, padding: 24 }}>
            <StatusBar style="auto" />
            <Text style={{ fontSize: 22, fontWeight: '600', marginBottom: 12 }}>
                Banana v2 (mobile)
            </Text>
            <Text style={{ fontSize: 12, color: '#555', marginBottom: 16 }}>
                API base: {API_BASE_URL || '<unset>'}
            </Text>
            <View style={{ flexDirection: 'row', alignItems: 'center', gap: 8 }}>
                <BananaBadge count={0}> (today)</BananaBadge>
                <RipenessLabel value="yellow" />
            </View>
        </SafeAreaView>
    );
}

registerRootComponent(App);
